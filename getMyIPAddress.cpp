#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "qotd.h"
/*-----------------------------------------------------------
 * Function: getMyIPAddress()
 *
 * Description: 
 *  Get the Local IP address using the following algorithm:
 *    - get local hostname with gethostname()
 *    - attempt to resolve local hostname with gethostbyname()
 *    if that fails:
 *    - get a UDP socket
 *    - connect UDP socket to arbitrary address and port
 *    - use getsockname() to get local address
 *  Returns a pointer to a null-terminated, dotted-decimal string
 */
char* getMyIPAddress() {
    char szLclHost [MAX_HOSTNAME];
    LPHOSTENT lpstHostent;
    SOCKADDR_IN stLclAddr;
    SOCKADDR_IN stRmtAddr;
	struct in_addr stIPAddr;
    int nAddrSize = sizeof(SOCKADDR);
    SOCKET hSock;
    int nRet;
    
    /* Init local address (to zero) */
    stLclAddr.sin_addr.s_addr = INADDR_ANY;
    
    /* Get the local hostname */
    nRet = gethostname(szLclHost, MAX_HOSTNAME); 
    if (nRet != SOCKET_ERROR) {
      /* Resolve hostname for local address */
      lpstHostent = gethostbyname((LPSTR)szLclHost);
      if (lpstHostent)
        stLclAddr.sin_addr.s_addr = *((u_long FAR*) (lpstHostent->h_addr));
    } 
    
    /* If still not resolved, then try second strategy */
    if (stLclAddr.sin_addr.s_addr == INADDR_ANY) {
      /* Get a UDP socket */
      hSock = socket(AF_INET, SOCK_DGRAM, 0);
      if (hSock != INVALID_SOCKET)  {
        /* Connect to arbitrary port and address (NOT loopback) */
        stRmtAddr.sin_family = AF_INET;
        stRmtAddr.sin_port   = htons(IPPORT_ECHO);
        stRmtAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        nRet = connect(hSock,
                       (LPSOCKADDR)&stRmtAddr,
                       sizeof(SOCKADDR));
        if (nRet != SOCKET_ERROR) {
          /* Get local address */
          getsockname(hSock, 
                      (LPSOCKADDR)&stLclAddr, 
                      (int FAR*)&nAddrSize);
        }
        closesocket(hSock);   /* we're done with the socket */
      }
    }
	stIPAddr.s_addr = stLclAddr.sin_addr.s_addr;
    return (inet_ntoa(stIPAddr));
} /* getMyIPAddress() */