#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define INF (unsigned short) 0xFFFF

using namespace std;

struct TableEntry{
    string destination,next_hop;
    unsigned short cost;
    TableEntry(string d,string n,unsigned short c){
        destination = d;next_hop = n;
        cost = c;
    }
};
struct Edge{
    string to;
    unsigned short cost;
    short up;
    Edge(string s,unsigned short c){to = s;cost = c;up = 3;}
};

//Global Data
set<string> unique_ips;
vector<TableEntry> routingTable;
vector<Edge> adjList;
string my_ip;
char network[20] = "192.168.10";

//networking
int sockfd;
int bind_flag;
int bytes_received;
socklen_t addrlen;
unsigned char buffer[1024];
struct sockaddr_in my_socket_address;
struct sockaddr_in recv_socket_address;

void print(vector<TableEntry> routingTable){
    cout << endl << setw(18) << left << "destination";
    cout << setw(18) << left << "next hop";
    cout << setw(8)  << left << "cost" << endl;

    cout << "---------------   " << "---------------   " << "--------" << endl;

    for(int i = 0;i < routingTable.size();i++){
        TableEntry a = routingTable[i];
        cout << setw(18) << left << a.destination;
        cout << setw(18) << left << a.next_hop;
        if(a.cost != INF)
        cout << setw(8)  << left << a.cost  << endl;
        else
        cout << setw(8)  << left << "INF" << endl;
    }
    cout << endl;
}
void print(vector<Edge> adjList){
    cout << "Neigobours" << endl;
    for(int i = 0;i < adjList.size();i++){
        Edge a = adjList[i];
        cout << a.to << " " << a.cost << endl;
    }
}

unsigned short getInitialCost(string destination){
    for(int i = 0;i < adjList.size();i++)
        if( (adjList[i].to == destination) && adjList[i].up )
            return adjList[i].cost;

    return INF;
}

void make_initial_routing_table(string own_ip){
    routingTable.clear();

    set<string>::iterator it = unique_ips.begin();
    while(it != unique_ips.end()){
        string dest = *it;
        if(dest != own_ip){
            unsigned short cost = getInitialCost(dest);
            routingTable.push_back(TableEntry(dest,cost == INF?"     -     ":dest,cost));
        }
        it++;
    }
}

void configureTopology(string own_ip,char *input){
    ifstream fin(input);
    string line;

    while(getline(fin,line)){
        unsigned short cost;
        char ip1[20],ip2[20];
        sscanf(line.c_str(),"%s%s%hu",ip1,ip2,&cost);
        string u = string(ip1);
        string v = string(ip2);

        if(u == own_ip)
            adjList.push_back(Edge(v,cost));
        else if(v == own_ip)
            adjList.push_back(Edge(u,cost));

        unique_ips.insert(u);unique_ips.insert(v);
    }
    fin.close();
    make_initial_routing_table(my_ip);
}

unsigned short bytesToShort(unsigned char msb,unsigned char lsb){
    unsigned short p,q;
    p = (unsigned short) msb;
    q = (unsigned short) lsb;
    return  (p << 8) + q;
}

void reset_all_routing_table(){
    make_initial_routing_table(my_ip);
    for(int i = 0;i < routingTable.size();i++){
        string ip = routingTable[i].destination;
        //sets up sending link
        recv_socket_address.sin_family = AF_INET;
        recv_socket_address.sin_port = htons(4747);
        recv_socket_address.sin_addr.s_addr = inet_addr(ip.c_str());

        //send reset message by buffer
        unsigned char buffer[1024] = {'r','e','s','e','t','\0'};
        sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &recv_socket_address, sizeof(sockaddr_in));
    }
}

void routing_table_asbytes_in(unsigned char buffer[]){
    ///encodes routing table as this format
    ///buffer[0]   -> r
    ///buffer[1]   -> t
    ///buffer[2]   -> top    8 bits of buffer size
    ///buffer[3]   -> bottom 8 bits of buffer size
    ///buffer[i]   -> last byte of ip
    ///buffer[i+1] -> top    8 bits of cost
    ///buffer[i+2] -> bottom 8 bits of cost
    int pos = 4;
    for(int i = 0;i < routingTable.size();i++){
        int a,b,c,d;
        sscanf(routingTable[i].destination.c_str(),"%d%*c%d%*c%d%*c%d",&a,&b,&c,&d);
        //printf("%d.%d.%d.%d\n",a,b,c,d);
        buffer[pos++] = d;//<last field of ip>
        buffer[pos++] = routingTable[i].cost >> 8;//<msb of cost>
        buffer[pos++] = routingTable[i].cost & 0x00FF;//<lsb of cost>
    }
    buffer[0] = 'r';
    buffer[1] = 't';
    buffer[2] = pos >> 8;
    buffer[3] = pos & 0x00FF;
}
void run_DRV_algo(unsigned char buffer[],string neighbour){
    unsigned short edge_cost;
    for(int i = 0;i < adjList.size();i++){
        if(adjList[i].to == neighbour){
            edge_cost = adjList[i].cost;

            if(!adjList[i].up){
                cout << endl << my_ip << " " << neighbour << " Link up again!" << endl;
                adjList[i].up = 3;
                reset_all_routing_table();
            }
            //makes the life 3
            adjList[i].up = 3;
            break;
        }
    }

    int buffer_size = bytesToShort(buffer[2],buffer[3]);
    for(int i = 4;i < buffer_size;i += 3){
        char ip[20];
        sprintf(ip,"%s.%d",network,(int)buffer[i]);
        string dest(ip);
        unsigned short cost = bytesToShort(buffer[i+1],buffer[i+2]);

        unsigned short new_cost = (cost == INF || edge_cost == INF)?(INF):(cost + edge_cost);
        for(int j = 0;j < routingTable.size();j++){
            if(routingTable[j].destination == dest){
                if(new_cost < routingTable[j].cost){
                    routingTable[j].cost     = new_cost;
                    routingTable[j].next_hop = neighbour;
                }
                break;
            }
        }
    }
}

void send_packet(unsigned char buffer[]){
    ///format send<192><168><10><0><192><168><10><0><lsb><msb><message>
    ///       0123  4    5   6   7   8    9   10 11  12   13    14-...

    //extracts message
    unsigned short message_length = bytesToShort(buffer[13],buffer[12]);
    //cout << "message_length = " << message_length << endl;

    char mess[0xFFFF];//mess array is at maximum size
    for(int i = 0;i < message_length;i++)
        mess[i] = buffer[14 + i];//14 + i -> because massage start at buffer[14]
    mess[message_length] = '\0';//terminates string
    string message(mess);
    //cout << "message = " << message << endl;

    //extracts destination ip
    int a = buffer[8]; int b = buffer[9];
    int c = buffer[10];int d = buffer[11];
    char ip[20];
    sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
    string dest(ip);
    //cout << "destination ip = " << dest << endl;

    if(dest == my_ip){
        cout << endl << message << " packet reached destination." << endl;
        return;
    }

    for(int i = 0;i < routingTable.size();i++){
        if(dest == routingTable[i].destination){
            string ip = routingTable[i].next_hop;
            cout << endl << message << " packet forwarded to " << ip << endl;
            //sets up sending link
            recv_socket_address.sin_family = AF_INET;
            recv_socket_address.sin_port = htons(4747);
            recv_socket_address.sin_addr.s_addr = inet_addr(ip.c_str());
            //send buffer
            sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &recv_socket_address, sizeof(sockaddr_in));
            return;
        }
    }
}

void update_cost(unsigned char buffer[]){
    //format cost<192><168><10><0><192><168><10><0><lsb><msb>
    //       0123  4    5   6   7   8    9   10  11  12   13
    //extracts destination ip
    int a,b,c,d;char ip[20];

    a = buffer[4]; b = buffer[5];
    c = buffer[6]; d = buffer[7];
    sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
    string u(ip);

    a = buffer[8] ; b = buffer[9];
    c = buffer[10]; d = buffer[11];
    sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
    string v(ip);

    unsigned short new_cost = bytesToShort(buffer[13],buffer[12]);

    string w;
    if(u == my_ip) w = v;
    else           w = u;
    for(int i = 0;i < adjList.size();i++){
        if(adjList[i].to == w){
            adjList[i].cost = new_cost;
            cout << endl << u << " " << v << " updated cost " << new_cost << endl;
            reset_all_routing_table();
            return;
        }
    }
}

void send_routing_table(unsigned char buffer[]){
    for(int i = 0;i < adjList.size();i++){
        string ip = adjList[i].to;
        //sets up sending link
        recv_socket_address.sin_family = AF_INET;
        recv_socket_address.sin_port = htons(4747);
        recv_socket_address.sin_addr.s_addr = inet_addr(ip.c_str());
        /*
            Link failure detection:
            every neighbour has an initial life (neighbour.up) = 3.
            for every call of send_routing_table() reduce the life by 1.
            for every call of run_DRV_algo() resets the life back to 3.
            so if for 3 consecutive session no update is recieved
            neighbour.up becomes 0,ei dead.
        */
        //if neighbour.up == 1 means the neighbour is not sending for 3 cycle(3,2,1)
        if(adjList[i].up == 1){
            cout << endl << my_ip << " " << ip << " Link down detected!" << endl;
            adjList[i].up = 0;
            reset_all_routing_table();
        }
        //reduce life
        if(adjList[i].up) adjList[i].up--;

        //load routing table in buffer
        routing_table_asbytes_in(buffer);
        //send buffer
        sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &recv_socket_address, sizeof(sockaddr_in));
    }
}

int main(int argc,char **argv){
    if(argc < 3){
        printf("Usage:\n");
        printf("./<file name> <ip address> <topology file>\n");
        exit(1);
    }
    ///argv[1] -> my ip
    ///argv[2] -> topology file
    my_ip = string(argv[1]);
    configureTopology(my_ip,argv[2]);

	//setup socket
	my_socket_address.sin_family = AF_INET;
	my_socket_address.sin_port = htons(4747);
	my_socket_address.sin_addr.s_addr = inet_addr(argv[1]);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1){
        printf("socket error!!\n");
        exit(1);
    }
	bind_flag = bind(sockfd, (struct sockaddr*) &my_socket_address, sizeof(sockaddr_in));
    if(bind_flag == -1){
        printf("bind error!!\n");
        exit(1);
    }

	cout << endl <<  my_ip << " up and running.." << endl;
	while(true){
        buffer[0] = '\0';

		bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &recv_socket_address, &addrlen);
		//cout << "[" << inet_ntoa(recv_socket_address.sin_addr)
		//	 << ":" << ntohs(recv_socket_address.sin_port) << "]  " << buffer << endl;

        if(buffer[0] == 'r' && buffer[1] == 'e'){//reset command
            //resets routing table
            make_initial_routing_table(my_ip);

		}else if(buffer[0] == 's' && buffer[1] == 'h'){//show command from driver.py
            //show routing table
			print(routingTable);

		}else if(buffer[0] == 'c' && buffer[1] == 'l'){//clock from driver.py
            //send routung table to neighbours
            send_routing_table(buffer);

		}else if(buffer[0] == 'r' && buffer[1] == 't'){//recieves routing table
            string neighbour = string(inet_ntoa(recv_socket_address.sin_addr));
            run_DRV_algo(buffer,neighbour);

        }else if(buffer[0] == 's' && buffer[1] == 'e'){//send command
            //send command response
			send_packet(buffer);

		}else if(buffer[0] == 'c' && buffer[1] == 'o'){//cost command from driver.py
            //updates cost
            update_cost(buffer);

		}
	}

    close(sockfd);
    return 0;
}
