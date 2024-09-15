// Complete me
#include "customparser.h"
#include <iostream>
#include <sstream>


using namespace std;

const char* error_msg_1 = "Cannot read integer n";
const char* error_msg_2 = "Error in content parsing";
const char* error_msg_3 = "Error in user parsing";

void CustomFormatParser::parse(std::istream& is, std::vector<Content*>& content, std::vector<User*>& users)
{
    
    content.clear();
    users.clear();

    

    int numContents=0, type=0, numSt, numRev, rate, numEps;
    int contentIdx;
    string nameOfContent;
    is>>numContents;
    if (is.fail())
      {
        throw ParserError(error_msg_1);
      }
    for (int i=0; i<numContents; i++)
    {
      cout <<"iteration i: " << i <<endl;
      is>>contentIdx>>type;
      cout<<"contentIdx:"<<contentIdx <<endl;
      cout<<"type:"<<type<<endl;
      if (is.fail())
      {
        cout <<"throwing content/type" <<endl;
        throw ParserError(error_msg_2);
      }
      getline(is, nameOfContent);
      is>>ws;
      getline(is, nameOfContent);
      is>>numRev>>numSt>>rate;
      if (is.fail())
      {
        throw ParserError(error_msg_2);
      }
      Content * contentptr;
      if(type==0){//movie
        contentptr = new Movie(contentIdx, nameOfContent, numRev, numSt, rate);
        content.push_back(contentptr);
      }
      else{//series
      is>>numEps;
      if (is.fail())
      {
        throw ParserError(error_msg_2);
      }
      contentptr = new Series(contentIdx, nameOfContent, numRev, numSt, rate, numEps);
        content.push_back(contentptr);
      }

      string userStr;
      getline(is, userStr);
      cout << "userStr: "<<userStr << endl;
      getline(is, userStr);
      cout << "userStr: "<<userStr << endl;
      stringstream ss(userStr);
      string temp;
      while(ss>>temp)
      {
        if (is.fail())
      {
        throw ParserError(error_msg_3);
      }
      
        contentptr->addViewer(temp);
      }

    }
    string userssss;
    int rate_lim;
    
    while (true)
    {
      is>>userssss;
      if(is.fail())
      {
        break;
      }
      is>>rate_lim;
      bool found=false;
      for (size_t i =0; i<users.size(); i++)
      {
        if (users[i]->uname==userssss)
        {
          found=true;
        }
      }
      if (found)
      {
      User * userptr = new User(userssss, rate_lim);
          users.push_back(userptr);
          getline(is, userssss);//dummy getline
          getline(is, userssss);
          stringstream xx(userssss);
          int c_id;
          while(xx>>c_id)
          {
            userptr->addToHistory(c_id);
          }
      }
      else{
        throw ParserError(error_msg_3);
      }
    }
    
}
