#pragma once
#include "Utilities.h"

LAMBDA_NAMESPACE_BEGIN

class LAMBDA_EXPORT Device {
	public:
		Device();

		Device(const Device &) = delete;

		Device &operator=(const Device &) = delete;
		

	private:
		class DeviceImp;
		DeviceImp *deviceImp;

		~Device();
};

LAMBDA_NAMESPACE_END