
#include "safer_string_view.h"

#include <string>
#include "gtest/gtest.h"

namespace dds_wrapper_unit_test
{
	TEST(NtStringView, StringCtor)
	{
		constexpr auto literal = "std::string initializer";
		std::string str{ literal };
		EXPECT_FALSE(dev::NtStringView::IsExplicitNt(str));		//view on std::string does not contain '\0' by default
		ASSERT_EQ(std::string_view(str).size(), 23);

		auto ntsv = dev::NtStringView(str);
		auto nt_str_view = ntsv.ViewApi();
		EXPECT_TRUE(dev::NtStringView::IsExplicitNt(ntsv));
		ASSERT_STREQ(ntsv.NullTerminatedCStr(), literal);
		EXPECT_EQ(nt_str_view.size(), 24);
		EXPECT_EQ(nt_str_view.back(), '\0');
	}

	TEST(NtStringView, StringLiteralCtor)
	{
		constexpr auto literal = "literal initializer";
		constexpr auto normal_sv = std::string_view{ literal };
		static_assert(!dev::NtStringView::IsExplicitNt(normal_sv), "normal sv should not have explicit null terminator");
		static_assert(19 == normal_sv.size());

		constexpr auto ntsv = dev::NtStringView("literal initializer");
		constexpr auto sv = ntsv.ViewApi();
		static_assert(dev::NtStringView::IsExplicitNt(ntsv), "NtStringView has explicit null terminator");
		ASSERT_TRUE(dev::NtStringView::IsExplicitNt(ntsv));
		static_assert(20 == sv.size());
		EXPECT_EQ(sv.size(), 20);
		static_assert(sv.substr(0, sv.size() - 1) == literal);
		ASSERT_STREQ(ntsv.NullTerminatedCStr(), literal);
		static_assert('\0' == sv.back());
		EXPECT_EQ(sv.back(), '\0');
	}

	TEST(NtStringView, CharTPCtor)
	{
		constexpr auto init = "sv initializer";
		static_assert(14 == std::string_view{init}.size());
		
		constexpr auto ntsv = dev::NtStringView(init);
		constexpr auto sv = ntsv.ViewApi();
		static_assert(dev::NtStringView::IsExplicitNt(ntsv), "NtStringView has explicit null terminator");
		ASSERT_TRUE(dev::NtStringView::IsExplicitNt(ntsv));
		static_assert(15 == sv.size());
		EXPECT_EQ(sv.size(), 15);
		static_assert('\0' == sv.back());
		EXPECT_EQ(sv.back(), '\0');
	}

	TEST(NtStringView, PtrLenCtor)
	{
		constexpr auto init = "sv initializer";
		static_assert(14 == std::string_view{ init }.size());

		constexpr auto ntsv = dev::NtStringView{ init, 14 };			//user attempts to construct without the null terminator
		constexpr auto sv = ntsv.ViewApi();
		static_assert(dev::NtStringView::IsExplicitNt(ntsv), "NtStringView has explicit null terminator");
		ASSERT_TRUE(dev::NtStringView::IsExplicitNt(ntsv));
		static_assert(15 == sv.size());
		EXPECT_EQ(sv.size(), 15);
		static_assert('\0' == sv.back());
		EXPECT_EQ(sv.back(), '\0');
	}

	TEST(NtStringView, SvCtor)
	{
		constexpr auto init = "sv initializer";
		static_assert(14 == std::string_view{ init }.size());

		constexpr auto ntsv = dev::NtStringView(std::string_view{ init });
		constexpr auto sv = ntsv.ViewApi();
		static_assert(dev::NtStringView::IsExplicitNt(ntsv), "NtStringView has explicit null terminator");
		ASSERT_TRUE(dev::NtStringView::IsExplicitNt(ntsv));
		static_assert(15 == sv.size());
		EXPECT_EQ(sv.size(), 15);
		static_assert('\0' == sv.back());
		EXPECT_EQ(sv.back(), '\0');
	}

	TEST(NtStringView, NntSV)
	{
		using namespace std::literals;
		constexpr auto haystack = "Haystack String"sv;
		constexpr auto needle = haystack.substr(haystack.find("stack"), 5);
		static_assert(0 == needle.compare("stack"));
		static_assert(haystack.size() == 15);

		EXPECT_THROW(dev::NtStringView ntsv{ needle }, std::runtime_error);
		//constexpr dev::NtStringView ntsv{ needle };		// wont compile - throws!
		EXPECT_NO_THROW(dev::NtStringView ntsv{ needle.data() });
		static_assert(13 == dev::NtStringView{ needle.data() }.ViewApi().size());

		constexpr static char arr[3] = { 't', 'r', 'y' };
		//constexpr std::string_view sv{ arr };				// wont compile - undefined behavior!
		//constexpr dev::NtStringView ntsv{ arr };			// wont compile - throws(but no undefined memory access!)
		EXPECT_THROW(dev::NtStringView ntsv{ arr }, std::runtime_error);


		dev::NtStringView ntsv{ needle.data() };
		dev::NtStringView ntsv2 = ntsv;
	}




	/*TEST(Basic, c_str)
	{
		constexpr auto literal = "test2";

		dev::NtStringView sv1(dev::is_null_terminated_t{}, literal);
		EXPECT_TRUE(sv1.IsNullTerminatedView());
		
		dev::NtStringView sv2(std::string_view{ literal });
		EXPECT_FALSE(sv2.IsNullTerminatedView());

		dev::NtStringView sv3(std::string_view{ literal, 6 });
		EXPECT_TRUE(sv3.IsNullTerminatedView());

		auto test = std::strlen(std::string_view{ literal }.data());
		std::cout << "len ex = " << test << ", len in = " << std::string_view{ literal, 6 }.size() << std::endl;
	}*/

	/*TODO arrange test cases::
		literal no tag
		literal with tag (over explicit but should be supported
		literal which can't be deduced - if successful will only work with tag
		sv of literal without \0
		sv of literal with \0
		string

	*/
	
	
	//	using namespace std::chrono_literals;
//	using namespace aura_pubsub;
//
//	TEST(StaticTest, RegisterOnce)
//	{
//		EXPECT_NO_THROW(DDSParticipantImpl::RegisterType<PingPong>());
//		EXPECT_THROW(rti::domain::register_type<PingPong>(), dds::core::Error);
//		EXPECT_NO_THROW((DDSParticipantImpl::RegisterType<PingPong, PingPong, PingPong>()));
//	}
//
//	TEST_F(APITest, IsReaderFound)
//	{
//		EXPECT_TRUE(tester_pubsub.IsReaderFound<PingPong>("PopularWriter"));
//		EXPECT_FALSE(tester_pubsub.IsReaderFound<PingPong>("LonelyWriter"));
//		ASSERT_EQ(1, tester_pubsub.ParticipantDomainId());
//	}
//
//	TEST_F(APITest, OnDataAvailable)
//	{
//		PingPong box(42);
//		PingPong gift(3);
//		tester_pubsub.RegisterOnDataAvailable<PingPong>("TakeReader", [&] {
//			auto msg = tester_pubsub.Take<PingPong>("TakeReader");
//			if (msg.has_value())
//			{
//				box = msg.value();
//			}
//			});
//		tester_pubsub.Write<PingPong>("PopularWriter", gift);
//		std::this_thread::sleep_for(500ms);
//
//		EXPECT_GT(42, box.Count());
//		EXPECT_EQ(box.Count(), 3);
//
//		box.Count() = 42;
//		tester_pubsub.UnregisterOnDataAvailable<PingPong>("TakeReader");
//		tester_pubsub.Write<PingPong>("PopularWriter", gift);
//		std::this_thread::sleep_for(500ms);
//
//		EXPECT_GT(box.Count(), gift.Count());
//		EXPECT_EQ(box.Count(), 42);
//		EXPECT_EQ(tester_pubsub.EmptyQueue<PingPong>("TakeReader"), 1);
//		ASSERT_FALSE(tester_pubsub.Take<PingPong>("TakeReader").has_value());
//	}
//
//	TEST_F(APITest, NonBlockingUnregister)
//	{
//		bool is_success = false;
//		tester_pubsub.RegisterOnDataAvailable<PingPong>("TakeReader", [&]()
//			{
//				ASSERT_NO_THROW(is_success = tester_pubsub.UnregisterOnDataAvailable<PingPong>("TakeReader"));
//				auto msg = tester_pubsub.Take<PingPong>("TakeReader");
//				ASSERT_TRUE(msg);
//				EXPECT_EQ(0, msg->Count());
//				
//			});
//		std::this_thread::sleep_for(std::chrono::milliseconds(100ms));
//
//		for (int j = 0; j < 5; ++j)
//		{
//			PingPong num{ uint32_t(j) };
//			tester_pubsub.Write("PopularWriter", num);
//			std::this_thread::sleep_for(100us);
//		}
//
//		ASSERT_TRUE(is_success);
//		EXPECT_EQ(tester_pubsub.EmptyQueue<PingPong>("TakeReader"), 4);
//		ASSERT_FALSE(tester_pubsub.Take<PingPong>("TakeReader").has_value());
//	}
//
//	TEST_F(APITest, Read)
//	{
//		EXPECT_GT(tester_pubsub.EmptyQueue<PingPong>("ReadReader"), 0);
//
//		PingPong boxes[3];
//		boxes[0].Count() = 10;
//		boxes[1].Count() = 50;
//		boxes[2].Count() = 0;
//
//		tester_pubsub.RegisterOnDataAvailable<PingPong>("ReadReader", [&] {
//			auto msg = tester_pubsub.Read<PingPong>("ReadReader");
//			if (msg.has_value())
//			{
//				boxes[2].Count() += msg.value().Count();
//			}
//			auto msg2 = tester_pubsub.Read<PingPong>("ReadReader");
//			if (msg2.has_value())
//			{
//				boxes[2].Count() += msg2.value().Count();
//			}
//			});
//
//		std::this_thread::sleep_for(500ms);
//		tester_pubsub.Write<PingPong>("PopularWriter", boxes[0]);
//		std::this_thread::sleep_for(500ms);
//
//		EXPECT_EQ(boxes[2].Count(), 20);
//		tester_pubsub.UnregisterOnDataAvailable<PingPong>("ReadReader");
//
//		EXPECT_GT(tester_pubsub.EmptyQueue<PingPong>("TakeReader"), 0);
//		ASSERT_FALSE(tester_pubsub.Take<PingPong>("TakeReader").has_value());
//	}
//
//	TEST_F(APITest, SynchronousTake)
//	{
//		EXPECT_EQ(std::nullopt, tester_pubsub.Take<PingPong>("TakeReader"));
//
//		PingPong gift{ 5 };
//		std::thread writer([&]() {
//			for (int counter = 0; counter < 20; ++counter)
//			{
//				if (counter % 3)
//				{
//					tester_pubsub.Write<PingPong>("PopularWriter", gift);
//					gift.Count()++;
//				}
//				std::this_thread::sleep_for(200us);
//			}});
//
//		int val = 5;
//		int misses = 0;
//		while (val < 18)
//		{
//			auto taken = tester_pubsub.Take<PingPong>("TakeReader");
//			if (taken.has_value())
//			{
//				EXPECT_EQ(taken.value().Count(), val++);
//			}
//			else
//			{
//				++misses;
//			}
//		}
//		EXPECT_GT(misses, 0);
//
//		writer.join();
//		tester_pubsub.EmptyQueue<PingPong>("TakeReader");
//	}
//
//	TEST_F(APITest, UnlockConcurrentDispatch)
//	{
//		constexpr int concurrency_size = 5;
//		DDSParticipantImpl::InitParams params{ "BasicTestLib::BasicTester",{ "unit_tests_config.xml"} };
//		params.thread_pool_size = concurrency_size;
//
//		DDSParticipantImpl participant(params);
//		std::array<int, concurrency_size> freelist{};
//		std::atomic<int> i{};
//
//
//		participant.RegisterOnDataAvailable<PingPong>("TakeReader", [&]()
//			{
//				auto msg = participant.Take<PingPong>("TakeReader");
//				//int idx = i.load();
//
//				if (!participant.UnlockConcurrentDispatch("TakeReader"))
//				{
//					std::cout << "ERROR!\n";
//				}
//				else if (msg.has_value())
//				{
//					std::this_thread::sleep_for(std::chrono::milliseconds(msg.value().Count() % 2 ? 20 - msg.value().Count() : 20 - msg.value().Count() * 5));
//					freelist[i.fetch_add(1)] = msg.value().Count();
//
//				}
//
//			});
//		std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
//		for (int j = 0; j < concurrency_size; ++j)
//		{
//			PingPong num{ uint32_t(j) };
//			participant.Write("PopularWriter", num);
//			std::this_thread::sleep_for(std::chrono::milliseconds(1));
//		}
//
//		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//
//		int test{};
//		for (int j = 0; j < concurrency_size + 1; ++j)
//		{
//			test = freelist[test];
//		}
//
//		EXPECT_EQ(freelist[0], test);
//	}
//
//	TEST_F(APITest, TimeoutErrors)
//	{
//		std::string reader("TakeReader");
//		tester_pubsub.RegisterOnDataAvailable<PingPong>(reader, [&]() {});
//		std::this_thread::sleep_for(std::chrono::milliseconds(10));
//
//		EXPECT_NO_THROW(tester_pubsub.Take<PingPong>(reader, 100ms));
//		ASSERT_TRUE(tester_pubsub.UnregisterOnDataAvailable<PingPong>(reader));
//
//		auto async_result = CallAsync([&]()
//			{
//				tester_pubsub.Read<PingPong>(reader, std::chrono::seconds(1));
//			});
//
//		std::this_thread::sleep_for(std::chrono::milliseconds(100));
//		EXPECT_THROW(tester_pubsub.Take<PingPong>(reader, std::chrono::seconds(5)), dds::core::PreconditionNotMetError);
//
//		ASSERT_TRUE(async_result.get());
//	}
//
//	TEST_F(APITest, TimeoutSynchronization)
//	{
//		std::string writer("PopularWriter");
//		std::string reader("TakeReader");
//		PingPong data(235);
//
//		auto async_result = CallAsync([&]()
//			{
//				std::this_thread::sleep_for(800ms);
//				tester_pubsub.Write(writer, data);
//			});
//
//		auto msg = tester_pubsub.Take<PingPong>(reader, 400ms);
//		ASSERT_FALSE(msg.has_value());
//
//		using Clock = std::chrono::system_clock;
//		Clock::time_point start = Clock::now();
//		msg = tester_pubsub.Read<PingPong>(reader, 500ms);
//		Clock::time_point stop = Clock::now();
//
//		ASSERT_TRUE(msg.has_value());
//		EXPECT_EQ(msg.value().Count(), 235);
//		EXPECT_GT(500ms, stop - start);
//		EXPECT_EQ(1, tester_pubsub.EmptyQueue<PingPong>(reader));
//	}
//
//	TEST_F(APITest, UserEvent)
//	{
//		int i = 42;
//		auto cb = ([&]()
//			{
//				++i;
//			});
//
//		aura_pubsub::UserEvent test(std::ref(cb));
//
//		tester_pubsub.RegisterOnRaisedEvent(test);
//
//		test.RaiseEvent();
//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
//		EXPECT_EQ(i, 43);
//
//		tester_pubsub.UnregisterOnRaisedEvent(test);
//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
//		test.RaiseEvent();
//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
//		EXPECT_NE(i, 44);
//	}
//
//	TEST_F(APITest, StringXml)
//	{
//		constexpr auto string_xml = "<dds>\n\t<domain_library name=\"BenchmarkDomainLibrary\" >\n\t\t<domain name=\"BenchmarkDomain\" domain_id=\"111\">\n\t\t\t<topic name=\"ping\" register_type_ref=\"PingPong\"/>\n\t\t\t<topic name=\"pong\" register_type_ref=\"PingPong\"/>\n\t\t\t<topic name=\"extra_type\" register_type_ref=\"MSG_PSP_DETECTION_REPORT\"/>\n\t\t\t<topic name=\"dwell_type\" register_type_ref=\"MSG_DWELL_SETTINGS_ANC\"/>\n\t\t</domain>\n\t</domain_library>\n\t\n\t<participant_library name=\"BasicTestLib\">\n\t\t<domain_participant name=\"BasicTester\" domain_ref=\"BenchmarkDomainLibrary::BenchmarkDomain\">\n\t\t\t<data_writer name=\"PopularWriter\" topic_ref=\"ping\"/>\n\t\t\t<data_writer name=\"LonelyWriter\" topic_ref=\"pong\"/>\n\t\t\t<data_reader name=\"ReadReader\" topic_ref=\"ping\"/>\n\t\t\t<data_reader name=\"TakeReader\" topic_ref=\"ping\"/>\n\t\t</domain_participant>\n\t</participant_library>\n</dds>";
//		aura_pubsub::DDSParticipantImpl::InitParams params{};
//		params.participant_name = "BasicTestLib::BasicTester";
//		params.xml_string_profiles.emplace_back(string_xml);
//
//		aura_pubsub::DDSParticipantImpl participant(params);
//		ASSERT_EQ(111, participant.ParticipantDomainId());
//	}
//
//	TEST_F(APITest, OverrideDomainId)
//	{
//		aura_pubsub::DDSParticipantImpl participant(aura_pubsub::DDSParticipantImpl::InitParams{ "BasicTestLib::BasicTester",{ "unit_tests_config.xml"}, {}, 33 });
//		ASSERT_EQ(33, participant.ParticipantDomainId());
//	}
//
//	TEST_F(APITest, MultiWrite)
//	{
//		tester_pubsub.ReaderEntity<PingPong>("TakeReader");
//		bool t1done = false;
//		bool t2done = false;
//		
//		std::thread writer([&]()
//		{
//			PingPong gift{ 5 };
//			for (int i{}; i < 1'000; ++i)
//			{
//				try
//				{
//					tester_pubsub.Write<PingPong>("PopularWriter", gift);
//				}
//				catch (const std::exception& ex)
//				{
//					std::cout << ex.what() << std::endl; 
//				}
//				std::this_thread::sleep_for(10us);
//			}
//			t1done = true;
//			std::cout << "writer1 finished\n";
//		});
//
//		std::thread writer2([&]()
//		{
//			PingPong gift{ 3 };
//			for (int i{}; i < 1'000; ++i)
//			{
//				try
//				{
//					tester_pubsub.Write<PingPong>("PopularWriter", gift);
//				}
//				catch (const std::exception& ex)
//				{
//					std::cout << ex.what() << std::endl;
//				}
//				std::this_thread::sleep_for(10us);
//			}
//			t2done = true;
//			std::cout << "writer2 finished\n";
//		});
//
//		int counter = 0;
//		while (!t1done || !t2done)
//		{
//			auto msg = tester_pubsub.Take<PingPong>("TakeReader");
//			if (++counter % 10000 == 0)
//			{
//				if (msg.has_value())
//					std::cout << msg.value().Count();
//				if (counter % 500000 == 0)
//					std::cout << std::endl;
//				else
//					std::cout << ", ";
//			}
//			std::this_thread::sleep_for(10us);
//		}
//
//		writer.join();
//		writer2.join();
//	}
//
//	TEST(Pingpong, threshold)
//	{
//		WrapperTest a_tester{};
//		std::this_thread::sleep_for(std::chrono::seconds(1));
//		EXPECT_EQ(a_tester.Test(30000), 30000);
//	}

} //namespace dds_wrapper_unit_test
