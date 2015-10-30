/******************************************************************************
 * Copyright (C) 2014 by Ralf Kaestner                                        *
 * ralf.kaestner@gmail.com                                                    *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the Lesser GNU General Public License as published by*
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the               *
 * Lesser GNU General Public License for more details.                        *
 *                                                                            *
 * You should have received a copy of the Lesser GNU General Public License   *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.       *
 ******************************************************************************/

#include <fstream>
#include <list>
#include <set>
#include <sstream>

#include <ros/package.h>

#include "variant_topic_tools/DataTypeRegistry.h"
#include "variant_topic_tools/Exceptions.h"
#include "variant_topic_tools/MessageDataType.h"
#include "variant_topic_tools/MessageDefinitionParser.h"
#include "variant_topic_tools/MessageType.h"
#include "variant_topic_tools/Publisher.h"
#include "variant_topic_tools/Subscriber.h"

namespace variant_topic_tools {

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

MessageType::MessageType(const std::string& dataType, const std::string&
    md5Sum, const std::string& definition) :
  dataType(dataType),
  md5Sum(md5Sum),
  definition(definition) {
}

MessageType::MessageType(const MessageDataType& dataType) :
  dataType(dataType.getIdentifier()),
  md5Sum(dataType.getMD5Sum()),
  definition(dataType.getDefinition()) {
}

MessageType::MessageType(const MessageType& src) :
  dataType(src.dataType),
  md5Sum(src.md5Sum),
  definition(src.definition) {
}

MessageType::~MessageType() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

void MessageType::setDataType(const std::string& dataType) {
  this->dataType = dataType;
}

const std::string& MessageType::getDataType() const {
  return dataType;
}

void MessageType::setMD5Sum(const std::string& md5Sum) {
  this->md5Sum = md5Sum;
}

const std::string& MessageType::getMD5Sum() const {
  return md5Sum;
}

void MessageType::setDefinition(const std::string& definition) {
  this->definition = definition;
}

const std::string& MessageType::getDefinition() const {
  return definition;
}

bool MessageType::isValid() const {
  return !md5Sum.empty() && ((md5Sum == "*") || (md5Sum.length() == 32)) &&
    !dataType.empty() && !definition.empty();
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void MessageType::load(const std::string& messageDataType) {
  clear();
  
  DataTypeRegistry registry;
  std::set<std::string> requiredTypes;
  std::list<std::string> requiredTypesInOrder;
  
  requiredTypes.insert(messageDataType);
  requiredTypesInOrder.push_back(messageDataType);
  
  while (!requiredTypesInOrder.empty()) {
    std::string package, type;
    std::string currentType = requiredTypesInOrder.front();
    
    size_t i = currentType.find_first_of('/');
    
    if ((i > 0) && (i != std::string::npos)) {
      package = currentType.substr(0, i);
      type = currentType.substr(i+1);
    }
    else
      type = currentType;
    
    if (package.empty()) {
      if (type == "Header")
        package = "std_msgs";
      else
        throw InvalidMessageTypeException(currentType);
    }
    
    if (type.empty())
      throw InvalidDataTypeException();
    
    std::string packagePath = ros::package::getPath(package);
    if (packagePath.empty())
      throw PackageNotFoundException(package);
    
    std::string messageFilename(packagePath+"/msg/"+type+".msg");
    std::ifstream messageFile(messageFilename.c_str());
    std::string messageDefinition;

    if (messageFile.is_open()) {
      messageFile.seekg(0, std::ios::end);   
      messageDefinition.reserve(messageFile.tellg());
      messageFile.seekg(0, std::ios::beg);
      
      messageDefinition.assign((std::istreambuf_iterator<char>(messageFile)),
        std::istreambuf_iterator<char>());
    }
    else
      throw FileOpenException(messageFilename);
    
    messageFile.close();
    
    if (!messageDefinition.empty()) {
      std::istringstream stream(messageDefinition);
      std::string line;
      
      while (std::getline(stream, line)) {
        std::string memberName, memberType;
        size_t memberSize;
        
        if (MessageDefinitionParser::matchArray(line, memberName, memberType,
            memberSize) ||  MessageDefinitionParser::match(line, memberName,
            memberType)) {
          if (memberType == "Header")
            memberType = "std_msgs/Header";
          
          if (!registry.getDataType(memberType).isBuiltin() &&
              requiredTypes.find(memberType) == requiredTypes.end()) {
            requiredTypes.insert(memberType);
            requiredTypesInOrder.push_back(memberType);
          }
        }
      }
      
      if (!definition.empty()) {
        definition += "\n"+std::string(80, '=')+"\n";
        definition += "MSG: "+currentType+"\n";
      }
      definition += messageDefinition;
    }
    
    requiredTypesInOrder.pop_front();
  }
  
  if (!definition.empty())
    dataType = messageDataType;
}

void MessageType::clear() {
  dataType.clear();
  md5Sum = "*";
  definition.clear();
}

void MessageType::write(std::ostream& stream) const {
  stream << dataType;
}

Publisher MessageType::advertise(ros::NodeHandle& nodeHandle,
    const std::string& topic, size_t queueSize, bool latch, const
    ros::SubscriberStatusCallback& connectCallback) {
  Publisher publisher;
  
  if (isValid())
    publisher.impl.reset(new Publisher::Impl(nodeHandle, *this, topic,
      queueSize, latch, connectCallback));

  return publisher;
}

Subscriber MessageType::subscribe(ros::NodeHandle& nodeHandle, const
    std::string& topic, size_t queueSize, const SubscriberCallback&
    callback) {
  Subscriber subscriber;
  
  subscriber.impl.reset(new Subscriber::Impl(nodeHandle, *this, topic,
    queueSize, callback));

  return subscriber;
}
  
/*****************************************************************************/
/* Operators                                                                 */
/*****************************************************************************/

bool MessageType::operator==(const MessageType& type) const {
  return (dataType == type.dataType) && (md5Sum == type.md5Sum);
}

bool MessageType::operator!=(const MessageType& type) const {
  return (dataType != type.dataType) || (md5Sum != type.md5Sum);
}

std::ostream& operator<<(std::ostream& stream, const MessageType&
    messageType) {
  messageType.write(stream);
}

}
