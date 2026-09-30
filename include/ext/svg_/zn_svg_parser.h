class ZN_SvgParser
{

public:
    ZN_SvgParser() = default;

    std::vector<ZN_SvgBundle> bundles;

    std::string svg_data;
    ZN_XmlParser xml;
    std::vector<ZN_XmlNode> groups;
    ZN_SvgParser(std::string svg_data, std::vector<ZN_XmlNode> groups, ZN_XmlParser xml) : svg_data(svg_data), groups(groups), xml(xml) {};

    std::vector<ZN_SvgBundle> to_bundles()
    {

        // printf("erwuouoiewr %s\n", xml.get_root().as_string().c_str());
        ZN_XmlNode root = xml.get_root()["defs"];
        // printf("\nerwuoiouerw %s %s\n",root.as_string().c_str(),"");

        for (int i = 0; i < groups.size(); i++)
        {
            ZN_XmlNode &g = groups[i];


            ZN_XmlNode n = root.first_where([&g](ZN_XmlNode node)
                                            { return node.linked_to(g); });

            ZN_SvgBundle b = ZN_SvgBundle::from_xml_node(n);
            b.copy_attr(g);
            bundles.push_back(b);
            // printf("erwouoiwer %f",b.args.x_advance);
        }


        std::reverse(bundles.begin(), bundles.end());



        return bundles;
    }

    static ZN_SvgParser from_string(std::string str)
    {
        ZN_XmlParser xml = ZN_XmlParser(str);

        ZN_XmlNode root = xml.get_root();

        std::vector<ZN_XmlNode> groups = root["g"].get_nodes();

        return ZN_SvgParser(str, groups, xml);
    }
};