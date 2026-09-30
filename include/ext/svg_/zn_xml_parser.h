
#include "tinyxml2.h"
using namespace tinyxml2;

class ZN_XmlNode
{
public:
    XMLElement *element;

    ZN_XmlNode() = default;
    ZN_XmlNode(XMLElement *e) : element(e) {};

    std::vector<ZN_XmlNode> get_nodes()
    {
        std::vector<ZN_XmlNode> nodes;
        iter_nodes([&nodes](ZN_XmlNode n)
                   {

            nodes.push_back(n);
            return true; });

        return nodes;
    }

    ZN_XmlNode operator[](std::string key)
    {
        ZN_XmlNode node;
        iter_nodes([&node, &key](ZN_XmlNode n)
                   {

                       if(std::string(n.get_tag_name()) == key){


                        node = n;
                           return false;

                       }


                       return true; });

        return node;
    }

    ZN_XmlNode operator[](char *key)
    {

        std::string _key = std::string(key);
        ZN_XmlNode node;
        iter_nodes([&node, &_key](ZN_XmlNode n)
                   {

                       if(std::string(n.get_tag_name()) == _key){


                        node = n;
                           return false;

                       }


                       return true; });

        return node;
    }

    const char *get_tag_name()
    {
        if (element)
            return element->Name();

        return nullptr;
    }

    const char *get_str_attr(char *name)
    {
        XMLElement *node = element;

        const char *val = node->Attribute(name);


        return val;
    }

     const float get_float_attr(char *name)
    {
        XMLElement *node = element;

        const float val = node->FloatAttribute(name);


        return val;
    }

    bool linked_to(ZN_XmlNode n2)
    {
        const char * href = n2.get_str_attr("href");
        const char * id = get_str_attr("id");

        href = href + 1; 
        return std::string(href) == std::string(id);
    }

    void iter_nodes(std::function<bool(ZN_XmlNode node)> next_node)
    {
        XMLElement *root = element;

        for (XMLElement *elem = root->FirstChildElement();
             elem != nullptr;
             elem = elem->NextSiblingElement())
        {

            ZN_XmlNode node(elem);
            bool _break = !next_node(node);

            if (_break)
                break;
        }
    }

    ZN_XmlNode first_where(std::function<bool(ZN_XmlNode)> next)
    {

        ZN_XmlNode node;

        iter_nodes([&node, next](ZN_XmlNode n)
                   {
                       bool _ok = next(n);

                       if (_ok)
                       {
                           node = n;
                           return false;
                       }

                       return true; });

        return node;
    }

    std::string as_string()
    {
        XMLPrinter printer;
        element->Accept(&printer);

        std::string str = printer.CStr(); // element + its children

        return str;
    }

    // void iter_nodes(std::function<ZN_XmlNode(ZN_XmlNode & node)> next_node)
    // {
    //     XMLElement *root = element;

    //     for (XMLElement *elem = root->FirstChildElement();
    //          elem != nullptr;
    //          elem = elem->NextSiblingElement())
    //     {

    //         ZN_XmlNode  node(elem);
    //         next_node(node);
    //     }
    // }
};

class ZN_XmlParser
{
private:
    int _parse()
    {

        doc = new XMLDocument();
        XMLError result = doc->Parse(svg_data.c_str());

        if (result != XML_SUCCESS)
        {
            printf("Failed to parse XML string\n");
            return 1;
        }

        return 0;
    }

    XMLElement *get_node(char *name)
    {
        return doc->FirstChildElement(name);
    }

    XMLElement *get_node(XMLElement *root, char *name)
    {
        return root->FirstChildElement(name);
    }

    void iter_node(XMLElement *root, char *name, std::function<bool(XMLElement *element)> next_element)
    {
        for (XMLElement *node = get_node(root, name); node; node = node->NextSiblingElement(name))
        {
            // next_element(node);
            // int id;
            // node->QueryIntAttribute("id", &id);
            // const char *name = user->GetText();
        }
    }

public:
    ZN_XmlParser() = default;
    XMLDocument *doc;

    // XMLElement &data; ///< Reference to TinyXML2 element

    std::string svg_data;

    ZN_XmlParser(std::string svg_data) : svg_data(svg_data)
    {
        _parse();
    };

    ZN_XmlNode get_root()
    {
        XMLElement *e = doc->FirstChildElement();

        ZN_XmlNode n(e);

        return n;
    }
};