#include "Communication.h"
#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;

Communication::Communication() : queueName("virtual")
{
	// Clear the queue:

}

Communication::~Communication()
{
}

void Communication::Clear()
{
	message_queue::remove(queueName.c_str());
}

Communication::Message Communication::GetQueuedMessage()
{
	try
	{
		Communication::Message message;
		unsigned int priority;
		message_queue::size_type recvd_size;

		message_queue mq(open_only, queueName.c_str());
		mq.receive(&message, sizeof(message), recvd_size, priority);

		if(recvd_size == sizeof(message))
		{
			return message;
		}
	}
	catch(interprocess_exception &ex)
	{
		return Communication::None;
	}
}

void Communication::SendQueuedMessage(Communication::Message message)
{
	message_queue mq(create_only, queueName.c_str(), 1, sizeof(Communication::Message));
	mq.send(&message, sizeof(message), 0);
}
