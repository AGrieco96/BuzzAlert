#include <stdio.h>
#include <stdlib.h>
#include "periph/gpio.h"
#include "xtimer.h"
#include "thread.h"
#include "periph/adc.h"
#include <math.h>

//Networking
#include "net/emcute.h"
#include "net/ipv6/addr.h"

// Macro for networks processes.
#define _IPV6_DEFAULT_PREFIX_LEN    (64U)
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)
#define NUMOFSUBS           (16U)
#define TOPIC_MAXLEN        (64U)
#define DEVICE_IP_ADDRESS   ("fec0:affe::99")
#define DEFAULT_INTERFACE   ("4")

#define MQTT_TOPIC_INT      "topic_board"
#define MQTT_TOPIC_EXT      "topic_data"
#define MQTT_QoS            (EMCUTE_QOS_0)

// Voltage Macros
#define RES ADC_RES_8BIT

//pins ultrasonic sensor

gpio_t trigger_pin = GPIO_PIN(PORT_A, 9); //D8 -> trigger
gpio_t echo_pin = GPIO_PIN(PORT_A, 8); //D7 -> echo

//pin Buzzer
gpio_t buzzer_pin = GPIO_PIN(PORT_C, 7); //D9 -> Buzzer

gpio_t led_pin = GPIO_PIN(PORT_B, 10); // D6 -> Led

//ultrasonic sensor
uint32_t echo_time;
uint32_t echo_time_start;

int past_read;
int current_state;



// Mosquitto Thing

static char stack[THREAD_STACKSIZE_DEFAULT];
static emcute_sub_t subscriptions[NUMOFSUBS];

// Emcute Thread 
static void *emcute_thread(void *arg){
    (void)arg;
    emcute_run(CONFIG_EMCUTE_DEFAULT_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}

// Message sending management -------------------

//Function called when a message is published on the topic the board is subscribed
static void on_pub(const emcute_topic_t *topic, void *data, size_t len){
    (void)topic;
    (void)len;
    char *in = (char *)data;

    char msg[len+1];
    strncpy(msg, in, len);
    msg[len] = '\0';
    printf("\nContenuto di msg : %s", msg);

 
    
    
    printf("### got publication for topic '%s' [%i] ###\n",
           topic->name, (int)topic->id);
    for (size_t i = 0; i < len; i++) {
        printf("%c", in[i]);
    }
    puts("");

    //Setting memory for the message buffer
    memset(in, 0, sizeof(char));
}

// Function for publish message to a topic
static int publish(char *t, char *message){
    emcute_topic_t topic;
    topic.name = t;
    
    //getting ID from the topic
    if(emcute_reg(&topic) != EMCUTE_OK){ 
        printf("\npublish : cannot find topic:%s ", topic.name);
        return 1;
    }

    //publishing on the broker
    if(emcute_pub(&topic, message, strlen(message), MQTT_QoS) != EMCUTE_OK){ 
        puts("\npublish : cannot publish data ");
        return 1;
    }
    printf("published message: %s (%i) on topic %s (id:%i)",
        message, (int)strlen(message), topic.name, topic.id);
    return 0;
}

// MQTT Initialization and subscription
static int sub(void){
    //Setup subscription
    subscriptions[0].cb = on_pub;
    subscriptions[0].topic.name = MQTT_TOPIC_INT;

    //Subscribing to topic
    if (emcute_sub(&subscriptions[0], MQTT_QoS) != EMCUTE_OK) {
        printf("\nconnect_broker: unable to subscribe to %s ", subscriptions[0].topic.name);
        return 1;
    }

    printf("\nsub: Now subscribed to %s ", subscriptions[0].topic.name);
    return 0;
}
static int connect_broker(void){

    printf("\nConnecting to MQTT-SN broker %s port %d. ",SERVER_ADDR, SERVER_PORT);

    // Socket creation 
    sock_udp_ep_t gw = { 
        .family = AF_INET6, 
        .port = SERVER_PORT 
    }; 

    char *topic = NULL;
    char *message = NULL;
    size_t len = 0;
    
    //Parsing IPv6 Address from String
    if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, SERVER_ADDR) == NULL) {
        puts("\nconnect_broker: error parsing IPv6 address ");
        return 1;
    }

    //Connecting to broker
    if (emcute_con(&gw, true, topic, message, len, 0) != EMCUTE_OK) {
        printf("\nconnect_broker: unable to connect to %s:%i ", SERVER_ADDR, (int)gw.port);
        return 1;
    }

    printf("\nSuccessfully connected to gateway at [%s]:%i ",SERVER_ADDR, (int)gw.port);

    return 0;
}

static uint8_t get_prefix_len(char *addr) {
    int prefix_len = ipv6_addr_split_int(addr, '/', _IPV6_DEFAULT_PREFIX_LEN);

    if(prefix_len < 1)
        prefix_len = _IPV6_DEFAULT_PREFIX_LEN;

    return prefix_len;
}
static int add_netif(char *name_if, char *dev_ip_address){

    netif_t *iface = netif_get_by_name(name_if); //getting the interface where to add the address
    if(!iface){
        puts("\nadd_netif: No valid Interface");
        return 1;
    }

    ipv6_addr_t ip_addr;
    uint16_t flag = GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID; 
    uint8_t prefix_len;
    
    prefix_len = get_prefix_len(dev_ip_address); 

    //Parsing IPv6 Address from String 
    if(ipv6_addr_from_str(&ip_addr, dev_ip_address) == NULL){
        puts("\nadd_netif: Error in parsing ipv6 address");
        return 1;
    }

    flag |= (prefix_len << 8U);

    //Set Interface Options 
    if(netif_set_opt(iface, NETOPT_IPV6_ADDR, flag, &ip_addr, sizeof(ip_addr)) < 0){
            puts("\nadd_netif: Error in Adding ipv6 address");
            return 1;
        }

    printf("\nadd_netif : Added %s with prefix %d to interface %s ", dev_ip_address, prefix_len, name_if);
    return 0;

}

static int setup_mqtt(void){

    //Setting memory for the subscription list
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));

    //Starting Emcute Thread
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,emcute_thread, NULL, "emcute");
    printf("\nEmcute Thread Started ");

    if(add_netif(DEFAULT_INTERFACE,DEVICE_IP_ADDRESS)){
        puts("\nsetup_mqtt: Faile to add network interface ");
        return 1;
    } 

    return 0;
}



// Sensors Stuff

void echo_cb(void* arg){ //callback function - ultrasonic sensor
	int val = gpio_read(echo_pin);
	uint32_t echo_time_stop;

    (void) arg;

	if(val){
		echo_time_start = xtimer_now_usec();
	}
    else{
		echo_time_stop = xtimer_now_usec();
		echo_time = echo_time_stop - echo_time_start;
	}
}


int read_distance(void){ //ultrasonic sensor
	echo_time = 0;
	gpio_clear(trigger_pin);
	xtimer_usleep(20);
	gpio_set(trigger_pin);
	xtimer_msleep(100);
	return echo_time;
}


void sensor_init(void){ //initializes all the pins

    gpio_init(trigger_pin, GPIO_OUT);
    gpio_init_int(echo_pin, GPIO_IN, GPIO_BOTH, &echo_cb, NULL); //imposta callback quando riceve input

    adc_init(ADC_LINE(0));

    if (gpio_init(led_pin, GPIO_OUT)) {
        printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_B, 10);
    }
    else{
        printf("OK to initialize GPIO_PIN(%d %d)\n", PORT_B, 10);
    }
    if (gpio_init(buzzer_pin, GPIO_OUT)) {
        printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_C, 7);
    }

    printf("FATTO %d", 7);
    read_distance(); //first read returns always 0
   
}




int main(void){

    // Broker Connection

    printf("\nSetting up MQTT-SN .  ");
    if(setup_mqtt()){
        printf("\nE: Something failed during the setup of MQTT");
        return 1;
    }
    
    printf("\nBroker Connection .  ");
    if(connect_broker()){
        printf("\nE: Something failed during the broker connection");
        return 1;
    }

    printf("\nStarting subscription .  ");
    if(sub()){
        printf("\nE: Something went wrong while subscribing to the topic ");
        
        return 1;
    }

    //Sensors Init

    xtimer_sleep(3);
    sensor_init();
    past_read=read_distance();
    
    
    
    
     while(1){
        
        int ultras = read_distance();
        
        printf("Distance from object : (%d)\n", ultras);
        printf("Past Distance from object : (%d)\n", past_read);
        xtimer_sleep(2);
        if ( abs(ultras - past_read)  > 1000){
            current_state = 1;      // Motion detected    --- ALARM!!! ---
            gpio_set(led_pin);
            gpio_set(buzzer_pin);
            xtimer_sleep(2);
            gpio_clear(led_pin);
            gpio_clear(buzzer_pin);
            xtimer_sleep(1);
        }
        if(current_state == 1){

            char message[80];
            sprintf(message,"%d:%d:%d", ultras, past_read,current_state);
            publish(MQTT_TOPIC_EXT, message); //publishing message on broker  

        }

        past_read = ultras;
        current_state = 0;          // Removing Alarm 
     }

    return 0;
}

/*
int main(void)
   printf("RIOT led_ext application\n"
           "Control an external LED using RIOT GPIO module.\n");

    gpio_t pin_out = GPIO_PIN(PORT_C, 7);
    if (gpio_init(pin_out, GPIO_OUT)) {
        printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_C, 7);
        return -1;
    }

    while (1) {
        printf("Set pin to HIGH\n");
        gpio_set(pin_out);

        xtimer_sleep(2);

        printf("Set pin to LOW\n");
        gpio_clear(pin_out);

        xtimer_sleep(2);
    }

    return 0;
    */
