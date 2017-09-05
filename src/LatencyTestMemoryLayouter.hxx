#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>

#include <hwloc.h>
//#include <hwloc/bitmap.h>
//#include <hwloc/cpuset.h>

#include "CompilerHelpers.hxx"
#include "LinkedList.hxx"

using namespace std;

class LatencyTestMemoryLayouter {
	//using create_f = std::unique_ptr< LatencyTestMemoryLayouter >();
	//typedef unordered_map< string, create_f * > type_registry;
	typedef unordered_map< string, LatencyTestMemoryLayouter * > type_registry;
private:
	size_t m_requested_len;

	static type_registry & registry();

	static const vector< string > split(const string& str, char delim = ' ') {
		vector< string > result;
		std::stringstream ss(str);
		std::string token;
		while (std::getline(ss, token, delim)) {
			result.push_back(token);
		}
		return result;
	};

public:
	virtual ~LatencyTestMemoryLayouter() DEF_DESTR;

	virtual size_t get_len(size_t m_requested_len) const = 0;
	virtual void initialize(LinkedListPtr mem, size_t len) const = 0;

	static void registerLayouter(LatencyTestMemoryLayouter *fp, string const & identifiers) {
		for(string identifier : split(identifiers, ',')) {
			if(registry().count(identifier) > 0) {
				cerr << "Layouter with identifier \"" << identifier << "\" already registered. Skipping." << endl;
			} else {
				registry()[identifier] = fp;
			}
		}
	}

	static LatencyTestMemoryLayouter *getLayouter(string identifier) {
		if(registry().count(identifier) != 1) {
			cerr << "Error: cannot find memory layouter with identifier \"" << identifier << "\"." << endl;
			return nullptr;
		}
		return registry()[identifier];
	}

	template <typename T>
	struct Registrar {
		bool initialized = false;

		explicit Registrar(string const & identifiers) {
			LatencyTestMemoryLayouter::registerLayouter(new T(), identifiers);
			initialized = true;
		}

		void ensureAlive(void) ATTR_USED { volatile bool _initialized = this->initialized; };
	};

	static void printLayouters(void) {
		cerr << "Layouters:" << endl;
		for(auto x : registry()) {
			cerr << x.first << endl;
		}
	}
};

