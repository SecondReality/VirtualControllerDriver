#pragma once

#include <string>

class Communication
{
public:
	Communication();
	~Communication();

	// Empty/Destroy the queue:
	void Communication::Clear();

	enum Message
	{
		None, StartController, StopController
	};

	Message GetQueuedMessage();
	void SendQueuedMessage(Message message);

private:
	std::string queueName;
};

