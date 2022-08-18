#pragma once

// base interface for OS specific opertations (threads, fileIO, GUI)
// each interface is a singleton 

namespace BASEINTERFACE {

	class IBaseInterface {
	public:
		virtual ~IBaseInterface() = 0;

		virtual int load() = 0;

		virtual int unload() = 0;
	};

	class InterfaceManager {
	public:
		int add_interface();
		int initialize_interfaces();
		int clear_up();
	};


};