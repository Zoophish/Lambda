#include "ShaderGraph.h"

LAMBDA_BEGIN
SG_BEGIN


void SocketRef::operator=(Socket *_rhs) {
	if (_rhs->socketType == socketType)
		socket = _rhs;
	else
		throw std::runtime_error("SocketRef::operator=() - Socket types do not match.");
}

bool Connect(SocketRef &_socketRef, const Socket &_socket) {
	if (_socketRef.socketType == _socket.socketType) {
		_socketRef.socket = (Socket *)&_socket;
		return true;
	}
	throw std::runtime_error("Connect() - socket types do not match.");
	return false;
}

void Disconnect(SocketRef &_socketRef) {
	_socketRef.socket = nullptr;
}

SG_END
LAMBDA_END