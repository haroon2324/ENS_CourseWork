#include <stdio.h>
#include <stdint.h> // Required for uint8_t


// Pre-processor Macros for a multi-byte array
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

int light_cycle(light *L, uint8_t mode){
    if(mode==1){
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
    else if(mode==2){
        L->R=250;
        L->G=250;
        L->B=250;
        L->light_seq=0;
        printf("master On activated \n");
    }
    else if(mode==3){
        L->R=0;
        L->G=0;
        L->B=0;
        L->light_seq=0;
        printf("master Off activated \n");
    }
    else{ 
        printf("Invalid Light cycle request \n");
    }
}

void status_checker(light *temp){
    printf("%s status: R:%u   G:%u   B:%u \n", temp->name,temp->R,temp->G,temp->B);
}

void app_initialise(uint16_t node1Address, uint8_t homeNode, uint16_t node2Address, uint16_t node3Address){
    //theres a 16 bit LIGHT ID value hardcoded from before into the nodes
    //assume 16 bit node address value
    
    switch(homeNode){
        B1 = (light){
            0,             //Light_sequence
            "B1",
            1,             //lightID
            node1Address,   //Home Address
            0,             //R
            0,             //G
            0};            //B
        L1 = (light){0,"L1",2,node1Address,0,0,0};
        B2 = (light){0,"B2",3,node2Address,0,0,0};
        K1 = (light){0,"K1",4,node3Address,0,0,0};
        E1 = (light){0,"E1",5,node3Address,0,0,0};
        case 1:
        global_home_node=homeNode;
        printf("Node 1 initialised \n");
        status_checker(&B1);
        status_checker(&L1);
        printf("Press B for Bedroom Switch! \n");
        break;
        case 2:
        //need to fill in and update later
        global_home_node=homeNode;
        printf("Node 2 initialised \n");
        status_checker(&B2);
        printf("Press M for Master Switch! \n");
        printf("Press K for Kitchen Switch! \n");
        break;
        case 3:
        //need to fill in and update later
        global_home_node=homeNode;
        printf("Node 3 initialised \n");
        status_checker(&K1);
        status_checker(&E1);
        printf("Press L for Living Room Switch! \n");
        break;
    }

}

uint8_t packer(uint8_t mode, light *temp2, uint8_t *app_packet){
    uint16_t tempID=temp2->light_ID;

    //Light ID, need to pack 16 bit value into 8 bit segments with bitshift and mask
    app_packet[1] = (uint8_t)((tempID >> 8) & 0xFF);  //High byte
    app_packet[2] = (uint8_t)(tempID & 0xFF);         //Low byte
    app_packet[3] = temp2->R;
    app_packet[4] = temp2->G;
    app_packet[5] = temp2->B;

    switch(mode){
        case 1:
        app_packet[0] = 0b10000000; //Set light mode
        break;
        case 2:
        app_packet[0] = 0b01000000; //Master On mode
        app_packet[3] = 250;
        app_packet[4] = 250;
        app_packet[5] = 250;
        break;
        case 3:
        app_packet[0]=0b00100000; //Master off mode
        app_packet[3] = 0;
        app_packet[4] = 0;
        app_packet[5] = 0;
        break; 
        
        default:
        return 0;
    }
        //app packet size
        return 6;
}
//need a function to control master on off for lights
//insert the scanf button press input here somehow whether its through
//ISR or whatever
void sender(char key){
    uint8_t app_pdu_holder[6];
    uint8_t packet_length;
    switch(global_home_node){
        case 1:
        if(key=='B' || key=='b'){
            printf("Bedroom switch pressed! \n");
            light_cycle(&B1,1);
            status_checker(&B1);
            light_cycle(&B2,1);
            packet_length = packer(1, &B2, app_pdu_holder);
            tran_request(B2.home_address,60, app_pdu_holder,packet_length);
            printf("Bedroom input packaged and sent to TRAN \n");
            }
        else{printf("invalid request \n");};
        break;
            
        case 2:
        if(key== 'K' || key == 'k'){
            printf("Kitchen switch pressed! \n ");
            light_cycle(&K1,1);
            packet_length= packer(1, &K1, app_pdu_holder);
            tran_request(K1.home_address,60,app_pdu_holder,packet_length);
            printf("Kitchen input Packaged and sent to TRAN \n");
            }
        else if(key == 'M' || key == 'm'){
            printf("Master switch pressed! \n");
            light_cycle(&B2,2);
            status_checker(&B2);
            packet_length=packer(2,&B1, app_pdu_holder);
            tran_request(B1.home_address,60,app_pdu_holder,packet_length);
            packet_length=packer(2,&L1, app_pdu_holder);
            tran_request(L1.home_address,60,app_pdu_holder,packet_length);
            packet_length=packer(2,&K1, app_pdu_holder);
            tran_request(K1.home_address,60,app_pdu_holder,packet_length);
            packet_length=packer(2,&E1, app_pdu_holder);
            tran_request(E1.home_address,60,app_pdu_holder,packet_length);
            printf("master inputs successfully sent \n");
                }
        else{printf("invalid request \n");};
        break;

        case 3:
        if(key== 'L' || key == 'l'){
            printf("Living room switch pressed! \n ");
            light_cycle(&L1,1);
            packet_length= packer(1, &L1, app_pdu_holder);
            tran_request(L1.home_address,60,app_pdu_holder,packet_length);
            printf("Living room input Packaged and sent to TRAN \n");
            }
        else{printf("invalid request \n");};
        break;
            //Send info to a packer function to make packet
        
        default:
        printf("No home node assigned \n");
        }
    }


