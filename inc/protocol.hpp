/*
 * =====================================================================================
 *
 *       Filename:  protocol.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/27/2018 03:59:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef SIMPLE_FTP_PROTOCOL_HEADER
#define SIMPLE_FTP_PROTOCOL_HEADER

void SendFile(Socket &s, const std::string&fileName);
void ReceiveFile(Socket &s, const std::string&fileName);
void ProtocolServer(Socket& s);
void ProtocolClient(Socket& s, char command, const std::string & fileName, std::string &fileList);

#endif //SIMPLE_FTP_PROTOCOL_HEADER
