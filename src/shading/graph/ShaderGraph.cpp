#pragma once
#include "ShaderGraph.h"

LAMBDA_BEGIN

SG_BEGIN

void SocketRef::operator=(Socket *_rhs) {
	socket = _rhs;
}



bool Connect(SocketRef &_socketRef, const Socket &_socket) {
	_socketRef.socket = (Socket *)&_socket;
	return true;
}

bool Connect(const Socket &_socket, SocketRef &_socketRef) {
	return Connect(_socketRef, _socket);
}

void Disconnect(SocketRef &_socketRef) {
	_socketRef.socket = nullptr;
}



Node::Node(const unsigned _numIn, const unsigned _numOut, const std::string &_nodeTag) :
	nodeTag(_nodeTag),
	numIn(_numIn), numOut(_numOut),
	inputSockets(new SocketRef[_numIn]),
	outputSockets(new Socket[_numOut]) {
		for (unsigned i = 0; i < numOut; ++i) outputSockets[i].node = this;
}

Node::Node(const Node &_node) :
	nodeTag(_node.nodeTag),
	numIn(_node.numIn), numOut(_node.numOut),
	inputSockets(new SocketRef[_node.numIn]),
	outputSockets(new Socket[_node.numOut]){
	for (unsigned i = 0; i < numIn; ++i) {
		inputSockets[i] = _node.inputSockets[i];
	}
	for (unsigned i = 0; i < numOut; ++i) {
		outputSockets[i] = _node.outputSockets[i];
		outputSockets[i].node = this;
	}
}

SocketRef *Node::GetInputSocket(const unsigned _index) {
	return &inputSockets[_index];
}

SocketRef *Node::GetInputSocket(const char *_tag) {
	for (unsigned i = 0; i < numIn; ++i) {
		if (inputSockets[i].tag == std::string(_tag)) return &inputSockets[i];
	}
}

SocketRef *Node::GetInputSocket(const SocketType _type) {
	for (unsigned i = 0; i < numIn; ++i) {
		if (inputSockets[i].socketType == _type) return &inputSockets[i];
	}
}

Socket *Node::GetOutputSocket(const unsigned _index) {
	return &outputSockets[_index];
}

Socket *Node::GetOutputSocket(const char *_tag) {
	for (unsigned i = 0; i < numIn; ++i) {
		if (outputSockets[i].tag == std::string(_tag)) return &outputSockets[i];
	}
}

Socket *Node::GetOutputSocket(const SocketType _type) {
	for (unsigned i = 0; i < numIn; ++i) {
		if (outputSockets[i].socketType == _type) return &outputSockets[i];
	}
}

GraphIterator Node::begin() {
	return GraphIterator(this);
}

GraphIterator Node::end() {
	return GraphIterator(nullptr);
}



GraphIterator::GraphIterator(Node *_root) {
	if (_root) {
		MakeStack(stack, _root);
		p = stack.top();
	}
}

GraphIterator::GraphIterator(const GraphIterator &_it) : GraphIterator(_it.p) {}

GraphIterator &GraphIterator::operator++() {
	stack.pop();
	if (!stack.empty()) p = stack.top();
	else p = nullptr;
	return *this;
}

GraphIterator GraphIterator::operator++(int) {
	GraphIterator it = *this;
	++(*this);
	return it;
}

bool GraphIterator::operator==(const GraphIterator &_rhs) const {
	return p == _rhs.p;
}

bool GraphIterator::operator!=(const GraphIterator &_rhs) const {
	return p != _rhs.p;
}

Node &GraphIterator::operator*() {
	return *p;
}

void GraphIterator::MakeStack(std::stack<Node *> &_stack, Node *_node) {
	_stack.push(_node);
	for (unsigned i = 0; i < _node->numIn; ++i) {
		MakeStack(_stack, _node->inputSockets[i].socket->node);
	}
}

SG_END

LAMBDA_END