#include "ismrmrd/version.h"
#include "ismrmrd/meta.h"
#include "pugixml.hpp"
#include "ismrmrd/vstypes.h"


namespace ISMRMRD {
    void deserialize(const char *xml, MetaContainer &h) {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_string(xml);

        if (!result) {
            throw std::runtime_error("Unable to load ISMRMRD Meta XML document");
        }

        pugi::xml_node root = doc.child("ismrmrdMeta");

        if (!root) {
            throw std::runtime_error("ismrmrdMeta tag not found in meta data header");
        }

        pugi::xml_node meta = root.child("meta");
        while (meta) {
            pugi::xml_node name = meta.child("name");
            pugi::xml_node value = meta.child("value");

            if (!name || !value) {
                std::runtime_error("Malformed metadata value");
            }

            while (value) {
                h.append(name.child_value(), value.child_value());
                value = value.next_sibling("value");
            }

            meta = meta.next_sibling("meta");
        }
    }

#ifndef PUGIXML_NO_STL
    void serialize(const MetaContainer &h, std::ostream &o) {
        pugi::xml_document doc;
        pugi::xml_node root = doc.append_child("ismrmrdMeta");
        typedef std::map<std::string, std::vector<MetaValue> > map_t;
        for (map_t::const_iterator node = h.begin(); node != h.end(); ++node) {
            pugi::xml_node meta = root.append_child("meta");
            pugi::xml_node name = meta.append_child("name");
            name.append_child(pugi::node_pcdata).set_value(node->first.c_str());
            for (unsigned int i = 0; i < node->second.size(); i++) {
                pugi::xml_node cname = meta.append_child("value");
                cname.append_child(pugi::node_pcdata).set_value(node->second[i].as_str());
            }
        }
        doc.save(o);
    }
#endif
}
