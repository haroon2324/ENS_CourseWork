#include <stdio.h>
#include <stdint.h> // Required for uint8_t


// Pre-processor Macros for a multi-byte array
// Adapted from code available on:
//https://www.cs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html

#define TURN_BIT_ON(array, bit)   (array[bit / 8] |= (1 << (bit % 8)))
#define TURN_BIT_OFF(array, bit)  (array[bit / 8] &= ~(1 << (bit % 8)))
#define CHECK_BIT(array, bit)     ((array[bit / 8] & (1 << (bit % 8))) ? 1 : 0)

uint16_t global_home_node;

typedef struct {
    uint8_t light_seq;
    char name[20];
    uint16_t light_ID;
    uint16_t home_address;
    uint8_t R;
    uint8_t G;
    uint8_t B;
} light;

light B1,B2,K1,E1,L1;

int light_cycle(light *L){
    switch(L->light_seq){
        case 1:
        L->R=250;
        L->G=0;
        L->B=0;
        L->light_seq++;
        printf("Cycle 1 \n");
        break;
        case 2:
        L->R=0;
        L->G=250;
        L->B=0;
        L->light_seq++;
        printf("Cycle 2 \n");
        break;
        case 3:
        L->R=0;
        L->G=0;
        L->B=250;
        L->light_seq++;
        printf("Cycle 3 \n");
        break;
        case 4:
        L->R=100;
        L->G=100;
        L->B=100;
        L->light_seq++;
        printf("Cycle 4 \n");
        break;
        case 5:
        L->R=0;
        L->G=225;
        L->B=225;
        L->light_seq=0;
        printf("Cycle 5 \n");
        break;
    }
}

void status_checker(light *temp){
    printf("%s status: R:%u   G:%u   B:%u \n", temp->name,temp->R,temp->G,temp->B);
}

void app_initialise(uint16_t homeAddress, uint8_t homeNode){
    //theres a 16 bit LIGHT ID value hardcoded from before into the nodes
    //assume 16 bit node address value
    
    switch(homeNode){
        case 1:
        global_home_node=homeNode;
        B1 = (light){
            0,             //Light_sequence
            "B1",
            1,             //lightID
            homeAddress,   //Home Address
            0,             //R
            0,             //G
            0};            //B
        L1 = (light){0,"L1",2,homeAddress,0,0,0};
        printf("Node 1 initialised \n");
        status_checker(&B1);
        status_checker(&L1);
        printf("Press B for Bedroom Switch! \n");
        break;
        case 2:
        //need to fill in and update later
        global_home_node=homeNode;
        light B2 = {0};
        B2.light_ID=2;
        B2.home_address=homeAddress;
        printf("Node 2 initialised \n");
        break;
        case 3:
        //need to fill in and update later
        global_home_node=homeNode;
        light K1 = {0};
        K1.light_ID=3;
        K1.home_address=homeAddress;
        light E1 = {0};
        E1.light_ID=4;
        E1.home_address=homeAddress;
        printf("Node 3 initialised \n");
        break;
    }

}

void packer(){

}

//insert the scanf button press input here somehow whether its through
//ISR or whatever
void sender(char key){
    switch(global_home_node){
        case 1:
        if(key=='B'){
            light_cycle(&B1);
            //Send info to a packer function to make packet
        }
    }
}