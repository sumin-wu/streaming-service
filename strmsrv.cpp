#include "strmsrv.h"
#include <iostream>
#include <fstream>
using namespace std;


StreamService::StreamService()
: users_(), content_()
{
    cUser_=nullptr;
}


StreamService::~StreamService()
{
    for (size_t i=0; i<users_.size(); i++)
    {
      delete users_[i];
    }
    for (size_t i=0; i<content_.size(); i++)
    {
      delete content_[i];
    }
}


void StreamService::readAndParseData(std::istream& is, Parser& p)
{
    p.parse(is, content_, users_);
    cout << "Read " << content_.size() << " content items." << endl;
    cout << "Read " << users_.size() << " users." << endl;
}


void StreamService::userLogin(const std::string& uname)
{

  if(cUser_==nullptr){
   for (size_t i=0; i<users_.size(); i++)
    {
      if (users_[i]->uname==uname)
      {
        cUser_=users_[i];
        break;
      }
    }
    if(cUser_==nullptr)
    {
      throw std::invalid_argument("not valid name");
    }
  }
  else{
    throw UserNotLoggedInError("User not logged out");
  }
  

}


void StreamService::userLogout()
{
  cUser_=nullptr;
}

std::vector<CID_T> StreamService::searchContent(const std::string& partial) const
{
    std::vector<CID_T> results;

    for(size_t i = 0; i < content_.size(); i++){
        // push back when the string 'partial'
        //  is contained in the name of the current content. Lookup the string
        //  class documentation to find an appropriate function that can be used
        //  to do this simply.

        if(partial == "*"){
            results.push_back(i);
        }
        else{
          bool found = true;
          for (unsigned int j=0; j<content_[i]->name().size(); j++)
          {
            if(content_[i]->name()[j]==partial[0])
            {
              int indx=j;
                for(unsigned int k=0; k<partial.size(); k++)
                {
                  if(content_[i]->name()[indx++]!=partial[k]) {
                    found=false;
                    break;
                }
                
              }
              if(found)
              {
                results.push_back(i);
                break;
              }
            }
          }
        }
    }
    return results;
}

std::vector<CID_T> StreamService::getUserHistory() const
{
    throwIfNoCurrentUser();
    return cUser_->history;
}

void StreamService::watch(CID_T contentID){
  if (cUser_==NULL)
  {
    throw UserNotLoggedInError("Not logged in");
  }
  else if(!isValidContentID(contentID)){
        throw range_error("Watch: invalid contentID");
    }
    
  else if (cUser_->ratingLimit < content_[contentID]->rating())
  {
    throw RatingLimitError("user is not allowed to watch this");
  }
  else
  {
    cUser_->addToHistory(contentID);
    content_[contentID]->addViewer(cUser_->uname);
  }
  
}


void StreamService::reviewShow(CID_T contentID, int numStars)
{
  if (cUser_==NULL)
  {
    throw UserNotLoggedInError("Not logged in");
  }
  else if(numStars<0||numStars>5)
  {
    throw ReviewRangeError("Rating limit error");
  }
  else if(! isValidContentID(contentID)){
        throw ReviewRangeError("Watch: invalid contentID");
    }
  else
  {
    content_[contentID]->review(numStars);
  }
}


CID_T StreamService::suggestBestSimilarContent(CID_T contentID) const
{
    vector<User*> people;
    for (size_t i=0; i<users_.size(); i++)
    {
      if(users_[i]!=cUser_)
      {
        for (size_t j=0; j<users_[i]->history.size(); j++)
        {
          if(users_[i]->history[j]==contentID)//find similar users who watched the same thing
          {
            people.push_back(users_[i]);
            break;
          }
        }
      }
    }

    if (people.empty())
    {
      return -1;
    }

    vector<CID_T> choices;
    for (size_t i=0; i<people.size(); i++)
    {
      for (size_t j=0; j<(people[i]->history).size(); j++)
      {
        if(!cUser_->haveWatched(people[i]->history[j]))
        {
          choices.push_back(people[i]->history[j]);
        }
      }
    }
    int maxxx=0, idxxx=-1;
    if (choices.empty())
    {
      return -1;
    }
    else if (choices.size()==1)
    {
      return choices.front();
    }
    else{
      
      for (size_t i=0; i<choices.size()-1; i++)
      {
        int cnt=1;
        if(choices[i]!=idxxx)
        {
          for (size_t j=i+1; j<choices.size();j++)
        {
          if(choices[j]==choices[i])
          {
            cnt++;
          }
        }
        if (cnt>maxxx){
          maxxx=cnt;
          idxxx=choices[i];
        }
        }
      }
    }
    return idxxx;
    
}


void StreamService::displayContentInfo(CID_T contentID) const
{
    
    if(! isValidContentID(contentID)){
        throw std::invalid_argument("Watch: invalid contentID");
    }
    else
    {
      content_[contentID]->display(cout);
    }

}

bool StreamService::isValidContentID(CID_T contentID) const
{
    return (contentID >= 0) && (contentID < (int) content_.size());
}


void StreamService::throwIfNoCurrentUser() const
{
    if(cUser_ == NULL){
        throw UserNotLoggedInError("No user is logged in");
    }
}


int StreamService::getUserIndexByName(const std::string& uname) const
{
    for(size_t i = 0; i < users_.size(); i++){
        if(uname == users_[i]->uname) {
            return (int)i;
        }
    }
    return -1;
}
