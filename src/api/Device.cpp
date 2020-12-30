#pragma once
#include <lambda/Device.h>

#include <core/Scene.h>
#include <assets/ResourceManager.h>

LAMBDA_NAMESPACE_BEGIN

class Device::DeviceImp {
	public:
	Scene scene;
	ResourceManager resourceManager;

	DeviceImp() {

	}
};

Device::Device() {
	deviceImp = new DeviceImp();
}

Device::~Device() {
	delete deviceImp;
}

LAMBDA_NAMESPACE_END