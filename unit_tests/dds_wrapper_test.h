
#pragma once

#include "dds_participant.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <future>

#include <gtest/gtest.h>

#include "pingpong.hpp"
#include "test_types/test_special_types.hpp"

inline auto& TesterPubsub()
{
	static aura_pubsub::DDSParticipantImpl tester_pubsub = []()
	{
		aura_pubsub::DDSParticipantImpl::RegisterType<PingPong, test_types::MsgBasicShmem>();
		return aura_pubsub::DDSParticipantImpl::InitParams{ "BasicTestLib::BasicTester",{ "unit_tests_config.xml"} };
	}();

	return tester_pubsub;
}

class APITest : public ::testing::Test
{
protected:
	APITest()
		: tester_pubsub{ TesterPubsub() }
	{
		static std::once_flag flag;
		std::call_once(flag, []()
			{
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(1s);
			});
	}

	static void SetUpTestSuite()
	{
	}

	static void TearDownTestSuite()
	{
		std::cout << "Tearing down APITest Suite\n";
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}

	aura_pubsub::DDSParticipantImpl& tester_pubsub;
};

template <class F>
std::future<bool> CallAsync(F&& fun)
{
	return std::async(std::launch::async, [&]()
		{
			try
			{
				fun();
				return true;
			}
			catch (const std::exception&)
			{
				return false;
			}
		});
}

class PingPublisher
{
public:
	PingPublisher(aura_pubsub::DDSParticipantImpl::InitParams commParams)
		: m_communicator(commParams)
		, m_pingMsg()
		, m_isRunning(false)
		, m_threshold()
	{
		m_communicator.RegisterOnDataAvailable<PingPong>(
			"pongReader", std::bind(&PingPublisher::PongCallback, this));
	}

	void Start(size_t threshold)
	{
		m_isRunning = true;
		m_threshold = threshold;

		while (!m_communicator.IsReaderFound<PingPong>("pingWriter"))
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		m_pingMsg.Count() = 0;
		m_communicator.Write<PingPong>("pingWriter", m_pingMsg);
	}

	void PongCallback()
	{
		auto msg = m_communicator.Take<PingPong>("pongReader");
		if (msg.has_value())
		{
			/*std::cout << "ping:" << */++msg.value().Count()/* << std::endl*/;
			m_communicator.Write<PingPong>("pingWriter", msg.value());
			if (m_threshold == msg.value().Count())
			{
				m_isRunning = false;
			}
		}
	}

	bool IsRunning()
	{
		return m_isRunning;
	}

private:
	aura_pubsub::DDSParticipantImpl m_communicator;
	PingPong m_pingMsg;
	bool m_isRunning;
	size_t m_threshold;
};

class PongPublisher
{
public:
	PongPublisher(const aura_pubsub::DDSParticipantImpl::InitParams& commParams, PingPublisher& a_ping);
	void PingCallback();
	bool IsRunning();

	PingPong m_pongMsg;
	bool m_isRunning;
private:
	aura_pubsub::DDSParticipantImpl m_communicator;
	PingPublisher& m_myPing;
};

inline PongPublisher::PongPublisher(const aura_pubsub::DDSParticipantImpl::InitParams& commParams, PingPublisher& a_ping)
	: m_communicator(commParams)
	, m_isRunning(true)
	, m_myPing(a_ping)
{
	m_communicator.RegisterOnDataAvailable<PingPong>(
		"pingReader", std::bind(&PongPublisher::PingCallback, this));
}

inline void PongPublisher::PingCallback()
{
	auto msg = m_communicator.Read<PingPong>("pingReader");
	if (msg.has_value())
	{
		if (m_myPing.IsRunning())
		{
			m_communicator.Write<PingPong>("pongWriter", msg.value());
		}
		else
		{
			m_pongMsg.Count() = msg.value().Count();
			m_isRunning = false;
		}
	}
}

inline bool PongPublisher::IsRunning()
{
	return m_isRunning;
}

class WrapperTest
{
public:

	WrapperTest()
		: m_ping(aura_pubsub::DDSParticipantImpl::InitParams{ "PingLib::Ping", {"unit_tests_config.xml"} })
		, m_pong(aura_pubsub::DDSParticipantImpl::InitParams{ "PongLib::Pong", {"unit_tests_config.xml"} }, m_ping)
	{}

	size_t Test(size_t threshold)
	{
		m_ping.Start(threshold);

		while (m_pong.IsRunning())
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		return m_pong.m_pongMsg.Count();
	}

private:
	PingPublisher m_ping;
	PongPublisher m_pong;
};
