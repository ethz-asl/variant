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

/** \file MessageType.h
  * \brief Header file providing the MessageType class interface
  */

#ifndef VARIANT_TOPIC_TOOLS_MESSAGE_TYPE_H
#define VARIANT_TOPIC_TOOLS_MESSAGE_TYPE_H

#include <variant_topic_tools/MessageDataType.h>

namespace variant_topic_tools {
  /** \brief Variant message type information
    */
  class MessageType {
  public:
    /** \brief Default constructor
      */ 
    MessageType(const std::string& dataType = std::string(),
      const std::string& md5Sum = "*", const std::string&
      definition = std::string());
    
    /** \brief Constructor (overloaded version accepting a message data type)
      */ 
    MessageType(const MessageDataType& dataType);
    
    /** \brief Copy constructor
      */ 
    MessageType(const MessageType& src);
    
    /** \brief Destructor
      */ 
    ~MessageType();

    /** \brief Set the data type of the message
      */ 
    void setDataType(const std::string& dataType);
    
    /** \brief Retrieve the data type of the message
      */ 
    const std::string& getDataType() const;
    
    /** \brief Set the MD5 sum of the message
      */ 
    void setMD5Sum(const std::string& md5Sum);
    
    /** \brief Retrieve the MD5 sum of the message
      */ 
    const std::string& getMD5Sum() const;
    
    /** \brief Set the message definition
      */ 
    void setDefinition(const std::string& definition);
    
    /** \brief Retrieve the message definition
      */ 
    const std::string& getDefinition() const;
    
    /** \brief True, if this message type is valid
      */
    bool isValid() const;

    /** \brief Write the message type to a stream
      */
    void write(std::ostream& stream, const std::string& indent =
      std::string()) const;
    
    /** \brief True, if this message type equals another message type
      */
    bool operator==(const MessageType& type) const;
    
    /** \brief True, if this message type does not equal another message type
      */
    bool operator!=(const MessageType& type) const;
    
  protected:
    /** \brief The data type of this message
      */ 
    std::string dataType;
    
    /** \brief The MD5 sum of this message
      */ 
    std::string md5Sum;
    
    /** \brief The definition of this message
      */ 
    std::string definition;
  };
  
  /** \brief Operator for writing the message type to a stream
    */
  std::ostream& operator<<(std::ostream& stream, const MessageType&
    messageType);
};

#endif
