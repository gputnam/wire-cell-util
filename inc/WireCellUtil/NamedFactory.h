#ifndef WIRECELL_NAMEDFACTORY_H
#define WIRECELL_NAMEDFACTORY_H

#include "WireCellUtil/IFactory.h"
#include "WireCellUtil/Singleton.h"
#include "WireCellUtil/PluginManager.h"
#include "WireCellUtil/Type.h"
#include <unordered_map>

#include <iostream>
#include <exception>
#include <string>

namespace WireCell {

    struct FactoryException : public std::exception 
    {
        std::string m_message;
        FactoryException(const std::string& msg = "NamedFactory lookup error") : m_message(msg) {}
        const char* what() const throw () {
            return m_message.c_str();
        }
    };

    /** A templated factory of objects of type Type that associates a
     * name to an object, returning a preexisting one if it exists. */
    template <class Type>
    class NamedFactory : public WireCell::INamedFactory {
    public:
	/// Remember the underlying type.
	typedef Type type;
	
	/// The exposed pointer type.
	typedef std::shared_ptr<Type> pointer_type;
	
	NamedFactory() : m_classname("") {}
	
	/// Return an instance associated with the given name.
	Interface::pointer create() { return create(""); }
	Interface::pointer create(const std::string& name) {
	    auto it = m_objects.find(name);
	    if (it == m_objects.end()) {
		pointer_type p(new Type);
		m_objects[name] = p;
		return p;
	    }
	    return it->second;
	}

	virtual void set_classname(const std::string& name) { m_classname=name; }
	virtual const std::string& classname() { return m_classname; }

    private:
	std::unordered_map<std::string, pointer_type> m_objects;
	std::string m_classname;
    };


    /** A registry of factories that produce instances which implement
     * a given interface. */
    template <class IType>
    class NamedFactoryRegistry {

    public:
	typedef IType interface_type;
	typedef std::shared_ptr<IType> interface_ptr;
	typedef WireCell::INamedFactory* factory_ptr;

	/// Register an existing factory by the "class" name of the instance it can create.
	bool associate(const std::string& classname, factory_ptr factory) {
	    m_lookup[classname] = factory;
	    return true;
	}

	/// Look up an existing factory by the name of the "class" it can create.
	factory_ptr lookup_factory(const std::string& classname) {
	    auto it = m_lookup.find(classname);
	    if (it != m_lookup.end()) {
		return it->second;
	    }

	    // cache miss, try plugin

	    WireCell::PluginManager& pm = WireCell::PluginManager::instance();

	    std::string factory_maker = "make_" + classname + "_factory";
	    auto plugin = pm.find(factory_maker.c_str());
	    if (!plugin) {
		std::cerr << "No plugin for " << classname << std::endl;
		return nullptr;
	    }

	    typedef void* (*maker_function)();
	    maker_function mf;
	    if (!plugin->symbol(factory_maker.c_str(), mf)) {
		std::cerr << "No factory maker symbol for " << classname << std::endl;
		return nullptr;
	    }

	    void* fac_void_ptr = mf();

	    if (!fac_void_ptr) {
		std::cerr << "No factory for " << classname << std::endl;
		return nullptr;
	    }

	    factory_ptr fptr = reinterpret_cast<factory_ptr>(fac_void_ptr);
	    m_lookup[classname] = fptr;
	    return fptr;
	}

	interface_ptr instance(const std::string& classname, const std::string& instname = "") {
	    factory_ptr fac = lookup_factory(classname);
	    if (!fac) {
		std::cerr << "No factory for class " << classname << std::endl;
		return nullptr;
	    }
	    WireCell::Interface::pointer iptr = fac->create(instname);
	    if (!iptr) {
		std::cerr << "Failed to create instance "<< instname <<" of class " << classname << std::endl;
		return nullptr;
	    }
	    interface_ptr uptype = std::dynamic_pointer_cast<interface_type>(iptr);
	    if (!uptype) {
		std::cerr << "Failed to cast "<< instname <<" of class " << classname
			  << " to " << type(uptype) << std::endl;
	    }
	    return uptype;
	}

    public:
	typedef std::unordered_map<std::string, factory_ptr> factory_lookup;
	factory_lookup m_lookup;
    };    

    /// Singleton interface
    namespace Factory {

	template<class IType>
	bool associate(const std::string& classname, WireCell::INamedFactory* factory) {
	    NamedFactoryRegistry<IType>&
		inst = WireCell::Singleton< NamedFactoryRegistry<IType> >::Instance();
            bool ok = inst.associate(classname, factory);
            if (!ok) { throw FactoryException("Failed to associate class " + classname); }
            return ok;
	}

	template<class IType>
	WireCell::INamedFactory* lookup_factory(const std::string& classname) {
	    NamedFactoryRegistry<IType>&
		inst = WireCell::Singleton< NamedFactoryRegistry<IType> >::Instance();
	    WireCell::INamedFactory* ret = inst.lookup_factory(classname);
            if (!ret) { throw FactoryException("Failed to lookup factory for " + classname); }
	    return ret;
	}

	template<class IType>
	std::shared_ptr<IType> lookup(const std::string& classname, const std::string& instname="") {
	    NamedFactoryRegistry<IType>&
		inst = WireCell::Singleton< NamedFactoryRegistry<IType> >::Instance();
	    std::shared_ptr<IType> ret = inst.instance(classname, instname);
            if (!ret) { throw FactoryException("Failed to lookup instance for " + classname + " " + instname); }
	    return ret;
	}
    }

} // namespace WireCell



    

#define WIRECELL_NAMEDFACTORY_BEGIN(CLASS)				\
    extern "C" { static void* gs_##CLASS##_factory; }			\
    extern "C" { void* make_##CLASS##_factory() {			\
    if (! gs_##CLASS##_factory) {					\
	gs_##CLASS##_factory = new WireCell::NamedFactory< CLASS >;	\
    }									\
    WireCell::NamedFactory< CLASS >* factory			\
       = reinterpret_cast< WireCell::NamedFactory< CLASS >* >(gs_##CLASS##_factory); \
    
#define WIRECELL_NAMEDFACTORY_INTERFACE(CLASS, INTERFACE)		\
    WireCell::Factory::associate<INTERFACE>(#CLASS, factory);


#define WIRECELL_NAMEDFACTORY_END(CLASS)	\
    return gs_##CLASS##_factory;		\
    }}


//WireCell::NamedFactory< CLASS >* gs_##CLASS##_factory;

#endif
