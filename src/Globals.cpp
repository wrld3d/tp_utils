#include "tp_utils/Globals.h"

#include <codecvt>
#include <locale>
#include <algorithm>

//##################################################################################################
std::string tpToHex(const std::string& input)
{
  static const char* const lut = "0123456789ABCDEF";

  size_t len = input.length();
  const uint8_t* s = reinterpret_cast<const uint8_t*>(input.data());
  const uint8_t* sMax = s + len;

  std::string output;
  output.reserve(2 * len);

  for (; s<sMax; s++)
  {
    const unsigned char c = *s;
    output.push_back(lut[c >> 4]);
    output.push_back(lut[c & 15]);
  }

  return output;
}

//##################################################################################################
std::string tpFromHEX(const std::string& input)
{
  static const char* const lut = "0123456789ABCDEF";
  size_t len = input.length();
  if(len & 1)
    return std::string();

  std::string output;
  output.resize(len / 2);

  uint8_t* o = reinterpret_cast<uint8_t*>(output.data());

  for (size_t i = 0; i < len; i+=2, o++)
  {
    char a = input[i];
    const char* p = std::lower_bound(lut, lut + 16, a);
    if(*p != a)
        return std::string();

    char b = input[i + 1];
    const char* q = std::lower_bound(lut, lut + 16, b);
    if(*q != b)
      return std::string();

    (*o) = uint8_t(((p - lut) << 4) | (q - lut));
  }

  return output;
}

//##################################################################################################
bool tpStartsWith(const std::string& input, const std::string& s)
{
  return input.size() >= s.size() && std::equal(s.begin(), s.end(), input.begin());
}

//##################################################################################################
bool tpEndsWith(const std::string& input, const std::string& s)
{
  return input.size() >= s.size() && std::equal(s.begin(), s.end(), input.end()-int(s.size()));
}

//##################################################################################################
namespace
{
void addPart(std::vector<std::string>& result, const std::string& input, size_t pos, size_t n, tp_utils::SplitBehavior behavior)
{
  if(behavior==tp_utils::SplitBehavior::SkipEmptyParts && n==0)
    return;

  result.push_back(input.substr(pos, n));
}
}

//##################################################################################################
void tpSplit(std::vector<std::string>& result,
             const std::string& input,
             const std::string& del,
             tp_utils::SplitBehavior behavior)
{
  std::string::size_type start = 0;
  auto end = input.find(del);
  while (end != std::string::npos)
  {
    addPart(result, input, start, end - start, behavior);
    start = end + del.length();
    end = input.find(del, start);
  }

  addPart(result, input, start, input.size()-start, behavior);
}

//##################################################################################################
void tpSplit(std::vector<std::string>& result,
             const std::string& input,
             char del,
             tp_utils::SplitBehavior behavior)
{
  std::string::size_type start = 0;
  auto end = input.find(del);
  while (end != std::string::npos)
  {
    addPart(result, input, start, end - start, behavior);
    start = end+1;
    end = input.find(del, start);
  }

  addPart(result, input, start, input.size()-start, behavior);
}

//##################################################################################################
void tpRemoveChar(std::string& s, char c)
{
  s.erase(std::remove_if(s.begin(), s.end(), [c](int a){return a==c;}), s.end());
}

//##################################################################################################
std::string tpToUTF8(const std::u16string& source)
{
  return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().to_bytes(source);
}

//##################################################################################################
std::u16string tpFromUTF8(const std::string& source)
{
  return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().from_bytes(source);
}

namespace tp_utils
{

//##################################################################################################
void leftJustified(std::string& text, size_t maxLength, char padding)
{
  if(text.size() >= maxLength)
    return;

  text+=std::string(maxLength-text.size(), padding);
}

//##################################################################################################
void rightJustified(std::string& text, size_t maxLength, char padding)
{
  if(text.size() >= maxLength)
    return;

  text = std::string(maxLength-text.size(), padding)+text;
}


//##################################################################################################
bool parseColor(const std::string& color, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
{
  r =   0;
  g =   0;
  b =   0;
  a = 255;

  if(color.size() != 7)
    return false;

  if(color.at(0)!='#')
    return false;

  uint32_t acc = 0;
  for(size_t i=1; i<7; i++)
  {
    char c = color.at(i);

    uint32_t v=0;
    if(c>='0' && c<='9')
      v = uint32_t(c-'0');

    if(c>='A' && c<='F')
      v = uint32_t(10 + (c-'A'));

    if(c>='a' && c<='f')
      v = uint32_t(10 + (c-'a'));

    acc<<=4;
    acc|=v;
  }

  b = uint8_t(acc); acc>>=8;
  g = uint8_t(acc); acc>>=8;
  r = uint8_t(acc);
  return true;
}

//##################################################################################################
bool parseColorF(const std::string& color, float& r, float& g, float& b, float& a)
{
  uint8_t r_;
  uint8_t g_;
  uint8_t b_;
  uint8_t a_;
  bool ret = parseColor(color, r_, g_, b_, a_);
  r = float(r_) / 255.0f;
  g = float(g_) / 255.0f;
  b = float(b_) / 255.0f;
  a = float(a_) / 255.0f;
  return ret;
}

}
