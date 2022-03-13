/**
 * @file meta.h
 * @defgroup meta Meta Attributes API
 * @{
 */

#ifndef ISMRMRDMETA_H
#define ISMRMRDMETA_H

#include "ismrmrd/export.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stdexcept>
#include <stdio.h>

namespace ISMRMRD {
    /*
      The serialized version of the structures would look like this

      <?xml version="1.0"?>
      <ismrmrdMeta>
         <!-- String value type -->
         <meta>
            <name>parameter1</name>
        <value>value_string</value>
         </meta>

         <!-- Integer value type -->
         <meta>
            <name>parameter1</name>
        <value>677797</value>
         </meta>

         <!-- Arrays can have mixed value types -->
         <meta>
            <name>parameter1</name>
        <value>1.456</value>
        <value>66797</value>
        <value>hsjdhaks</value>
         </meta>
      </ismrmrdMeta>
     */



    /**
       This class can represent a meta data value of any
       type and it guarantees that any value will have a
       representation as any type.

       The class uses std::string internally to store the
       string representation of the value but this std::string
       is never exposed on the class interface and so it should not
       need to be exported in Windows. For now, this class can be header only.
     */
    class MetaValue {

    public:
        /** Default constructor */
        MetaValue() {
            set(0L);
        }

        ///Null terminated string constructor
        MetaValue(const char *s) {
            set(s);
        }

        ///Long constructor
        MetaValue(long l) {
            set(l);
        }

        ///Long constructor
        MetaValue(double d) {
            set(d);
        }


        ///Assignment operator for string
        MetaValue &operator=(const char *s) {
            set(s);
            return *this;
        }

        ///Assignment operator for long
        MetaValue &operator=(long l) {
            set(l);
            return *this;
        }

        ///Assignment operator for double
        MetaValue &operator=(double d) {
            set(d);
            return *this;
        }

        ///Get the ingeter representation of the value
        long as_long() const {
            return l_;
        }

        ///Get the floating point representation of the value
        double as_double() const {
            return d_;
        }

        ///get the C string representation of the value
        const char *as_str() const {
            return s_.c_str();
        }


    protected:
        long l_;
        double d_;
        std::string s_;

        void set(const char *s) {
            s_ = std::string(s);
            sscanf(s_.c_str(), "%ld", &l_);
            sscanf(s_.c_str(), "%lf", &d_);
        }

        void set(long l) {
            l_ = l;
            d_ = static_cast<double>(l_);
            std::stringstream strstream;
            strstream << l_;
            strstream >> s_;
        }

        void set(double d) {
            d_ = d;
            l_ = static_cast<long>(d_);
            std::stringstream strstream;
            strstream << d_;
            strstream >> s_;
        }
    };

    class MetaContainer;

    EXPORTISMRMRD void deserialize(const char *xml, MetaContainer &h);

    EXPORTISMRMRD void serialize(const MetaContainer &h, std::ostream &o);

    /// Meta Container
    class MetaContainer {
    protected:
        typedef std::map<std::string, std::vector<MetaValue> > map_t;

    public:
        MetaContainer() {

        }

        /**
           This function sets the parameter with name @name to
           value @value. The set function assumes the parameter
           will have a single value and wipes out any array that might be there.

           There is an @append function for appending to an existing array.
         */
        template<class T>
        void set(const char *name, T value) {
            MetaValue v(value);
            map_[std::string(name)] = std::vector<MetaValue>(1, value);
        }


        template<class T>
        void append(const char *name, T value) {
            map_t::iterator it = map_.find(std::string(name));
            if (it == map_.end()) {
                set(name, value);
            } else {
                MetaValue v(value);
                it->second.push_back(v);
            }
        }

        void remove(const char *name) {
            map_t::iterator it = map_.find(std::string(name));
            if (it != map_.end()) {
                map_.erase(it);
            }
        }

        /// Return number of values of a particular parameter
        size_t length(const char *name) const {
            map_t::const_iterator it = map_.find(std::string(name));
            if (it != map_.end()) {
                return it->second.size();
            }
            return 0;
        }

        /// Return value number @index of the parameter @name as long
        long as_long(const char *name, size_t index = 0) const {
            return value(name, index).as_long();
        }

        /// Return value number @index of the parameter @name as double
        double as_double(const char *name, size_t index = 0) const {
            return value(name, index).as_double();
        }

        /// Return value number @index of the parameter @name as string
        const char *as_str(const char *name, size_t index = 0) const {
            return value(name, index).as_str();
        }

        const MetaValue &value(const char *name, size_t index = 0) const {
            map_t::const_iterator it = map_.find(std::string(name));
            if (it == map_.end()) {
                throw std::runtime_error("Attempting to access unknown parameter");
            }
            if (index >= it->second.size()) {
                throw std::runtime_error("Attempting to access indexed value out of bounds");
            }
            return it->second[index];
        }

        bool empty() const {
            return map_.empty();
        }

        map_t::iterator begin() {
            return map_.begin();
        }

        map_t::iterator end() {
            return map_.end();
        }

        map_t::const_iterator begin() const  {
            return map_.begin();
        }

        map_t::const_iterator end() const  {
            return map_.end();
        }


    protected:
        map_t map_;
    };

    /** @} */

    //Template function instantiations
    /*
    template void MetaContainer::set<const char*>(const char* name, const char* value);
    template void MetaContainer::set<long>(const char* name, long value);
    template void MetaContainer::set<double>(const char* name, double);
    template void MetaContainer::append<const char*>(const char* name, const char* value);
    template void MetaContainer::append<long>(const char* name, long value);
    template void MetaContainer::append<double>(const char* name, double);
    */
}


#endif //ISMRMRDMETA_H
