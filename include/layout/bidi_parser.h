
class BidiParser
{

public:
    static const char * parse(const char *text)
    {

         const char *out;

       const char * t =  parse_bidi_text(text, &out);

        // printf("erwouiuoiwer %s. xc\n",t);
        // return std::string(t);


        return out;
    }
};