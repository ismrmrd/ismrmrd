#include "ismrmrd_meta.h"
#include "pugixml.hpp"


namespace ISMRMRD
{
  void deserialize(const char* xml, MetaContainer& h)
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load(xml);
    //TODO should something be done with result?
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
	h.append(name.child_value(),value.child_value());
	value = value.next_sibling("value");
      }

      meta = meta.next_sibling("meta");
    }
  }

  void serialize(MetaContainer& h, std::ostream& o)
  {
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("ismrmrdMeta");
    
    MetaContainer::map_t::iterator it = h.map_.begin();
    while (it != h.map_.end()) {
      pugi::xml_node meta = root.append_child("meta");
      pugi::xml_node name = meta.append_child("name");
      name.append_child(pugi::node_pcdata).set_value(it->first.c_str());
      for (unsigned int i = 0; i < it->second.size(); i++) {
	pugi::xml_node name = meta.append_child("value");
	name.append_child(pugi::node_pcdata).set_value(it->second[i].as_str());
      }
      it++;
    }
    doc.save(o);
  }

}
