// parser.hpp
//
// Provides functions to parse HTML content and deliver 
//
// 2019-10-26: Created get_links function to get urls from basic html:
//             medhak, tjha

#include "../utils/basicString.hpp"
#include "../utils/vector.hpp"
#include "../utils/algorithm.hpp"
#include "../utils/exception.hpp"
#include <iostream>

using dex::string;
using dex::vector;
using dex::exception;

using std::cout;
using std::endl;

vector<string> get_links(string html_file)
   {
   int pos_open_tag = 0, pos_close_tag = 0;
   string url;
   vector <string> urlList;
  
   while ( pos_open_tag != -1 )
      {
      pos_close_tag = html_file.find(">", pos_open_tag);
      if (html_file[ pos_open_tag + 1 ] == '!')
         {
         pos_close_tag = html_file.find("-->", pos_open_tag);
         pos_close_tag += 2;
         }
      else
         {
         if (pos_close_tag != -1)
            {
            int pos_href = html_file.find("href", pos_open_tag);
            if (pos_href < pos_close_tag && pos_href != -1)
               {
               if (html_file.find("a", pos_open_tag) < pos_href)
                  {
                  int pos_temp1 = html_file.find("=", pos_href);
                  if (pos_temp1 != -1)
                     {
                     url = html_file.substr(pos_temp1 + 1, pos_close_tag-pos_temp1-1);               
                     if (url[0] == '\"')
                        {
                        url = url.substr(1, url.length()-2);
                        urlList.pushBack(url);
                        }
                     }
                  
                  }
               }
            }
         }
      pos_open_tag = html_file.find("<", pos_close_tag);   
      
      }
   return urlList;
   }
   
