#include "gc.hpp"

#ifdef DEBUG_GC
#include <iostream>
#endif

namespace GC {
	namespace {
		std::unordered_set<GCObject*> objects;
		std::unordered_map<GCObject*, int> rootObjects;
		
		uint32_t nextCollect = 16;
		
		void add(GCObject* obj) {
			objects.insert(obj);
		}
	}
	
	void pin(GCObject* obj) {
		IF_DEBUG_GC(std::cout << "Pinning " << obj << std::endl;)
		if(rootObjects.find(obj) == rootObjects.end())
			rootObjects[obj] = 0;
		rootObjects[obj]++;
	}
	
	void unpin(GCObject* obj) {
		IF_DEBUG_GC(std::cout << "Unpinning " << obj << std::endl;)
		if(--rootObjects[obj] <= 0)
			rootObjects.erase(obj);
	}
	
	void logState() {
		IF_DEBUG_GC(std::cout << "GC contains " << objects.size() << " objects (" << rootObjects.size() << " roots)" << std::endl;)
	}
	
	void collect() {
		IF_DEBUG_GC(std::cout << "Collecting..." << std::endl;)
		
		for(auto pair : rootObjects) {
			pair.first->mark();
		}
		
		auto it = objects.begin();
		while(it != objects.end()) {
			GCObject* obj = *it;
			if(obj->isMarked()) {
				obj->reset();
				++it;
			} else {
				it = objects.erase(it);
				delete obj;
			}
		}
		
		IF_DEBUG_GC(std::cout << "Done collecting." << std::endl;)
		logState();
	}
	
	void step() {
		if(objects.size() >= nextCollect) {
			GC::collect();
			nextCollect = objects.size() * 2;
		}
	}
	
	GCObject::GCObject() : _marked(false) {
		IF_DEBUG_GC(std::cout << "Created GCObject " << this << std::endl;)
		add(this);
	}
	
	GCObject::~GCObject() {
		IF_DEBUG_GC(std::cout << "Deleted GCObject " << this << std::endl;)
	}
	
	void GCObject::mark() {
		if(!_marked) {
			_marked = true;
			markChildren();
		}
	}
	
	void GCObject::reset() {
		_marked = false;
	}
	
	bool GCObject::isMarked() { return _marked; }
	
	void GCObject::markChildren() {}
}
