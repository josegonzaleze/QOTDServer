//Jose Gonzalez

#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <string>

using namespace std;

#pragma comment (lib, "ws2_32.lib")
SOCKET connectsock(char *host, char *service, char *protocol);

int UDP_send(SOCKET, char*, int, char*, char*);
int UDP_recv(SOCKET, char*, int, char*, char*);


const int MAXNAME = 80;
const int maxNumberOfBytes = 2048;

SOCKET passivesock(char *service, char *protocol);
char* timestamp();
char* getMyIPAddress();

void main() {

	//Initialize the Winsock library (WSAStartup).
	WORD wVersionRequired = 0x0101;
	WSADATA wsaData;
	int err = WSAStartup(wVersionRequired, &wsaData);

	if (err) {

		cout << "Unable to initialize Windows Socket library. " << endl;
		return;
	}

	//Create a UDP socket using passivesock() and output the startup message to the log.
	SOCKET s = passivesock("17", "UDP");

	if (s != INVALID_SOCKET) {


		cout << timestamp() << " - Start qotd service on " << getMyIPAddress() << endl;
		
		//Perform the following actions until receiving a valid shutdown request:
			//a.Wait for a client to send a datagram
		char buffer[maxNumberOfBytes];
		char remoteHost[MAXNAME];
		char remotePort[MAXNAME];
		int len;
		bool shutdown = false;

		while(!shutdown){

			len = UDP_recv(s, buffer, maxNumberOfBytes - 1, remoteHost, remotePort);
			
			//b.Using the client’s IP address and port number report the connection to the log.			
			cout << timestamp() << " - Datagram received from: " << remoteHost << endl;

			//c. If the received message is “sendQOTD” (case insensitive), open the qotd.txt file and send it to
				//the client, using its IP address and port number(and report to log).			
			string MessageReceived;
			for (int i = 0; i < len; i++) {
				MessageReceived += buffer[i];
			}
			if ((_stricmp(MessageReceived.c_str(), "SENDQOTD") == 0)) {

				cout << timestamp() << " - Received command: " << buffer << endl;

				/*
				c. If the received message is “sendQOTD” (case insensitive), open the qotd.txt file and send it to
				the client, using its IP address and port number (and report to log).
				*/				
				ifstream fin;
				fin.open("qotd.txt");
				string QOTD;

				//Get all the text in one string.
				while (!fin.eof()) {

					string temp;
					getline(fin,temp);
					QOTD += temp;
					QOTD += '\n';
				}

				//After having the text, closing the file.
				fin.close();

				//Converting string to char *				
				char *qotd = new char[QOTD.length() + 1];
				strcpy(qotd, QOTD.c_str());

				//sending the file's text and report to log
				UDP_send(s, qotd, QOTD.length() + 2, remoteHost, remotePort);
				cout << timestamp() << " - Sent file: qotd.txt to " << remoteHost << endl;

				/*
				d. If the received message is “shutdown” and it was sent from the same machine as that running the
				server, send the client the message: "Service: qotd - will be shutdown" and set a flag to
				exit the loop. Otherwise send the client a deny message: "SHUTDOWN request denied". You
				should also log the message.
				*/
			}else if((_stricmp(MessageReceived.c_str(), "shutdown") == 0)){


				if (_stricmp(remoteHost, getMyIPAddress()) == 0) {

					cout << timestamp() << " - Received command: " << buffer << endl;
					cout << timestamp() << " - Service: qotd - will be shutdown" << endl;
					char* log = "Service: qotd - will be shutdown";
					UDP_send(s, log, 32, remoteHost, remotePort);
					shutdown = true;

				}
				else {
					
					cout << timestamp() << " - Ignored command: " << buffer << endl;
					char* log = "SHUTDOWN request denied";
					UDP_send(s, log, 23, remoteHost, remotePort);


				}
			}
			else {

				cout << timestamp() << " - Ignored command: " << buffer << endl;
			}
		}

		//Close the socket (and thus the connection).
		closesocket(s);
	}
	//At this point, s in not a valid socket
	else {

		cout << "Error creating the UPD socket, check:  --passivesock()-- " << endl;
	}
	
	WSACleanup();
}
