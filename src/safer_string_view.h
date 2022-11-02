
#ifndef SAFER_STRING_VIEW_H
#define SAFER_STRING_VIEW_H

#include <string_view>				//std::string_view
#include <stdexcept>				//std::runtime_error
#include <type_traits>				//std::is_constructible
#include <utility>					//std::forward


namespace dev
{
	namespace detail
	{
		template<typename... Args>
		using EnableIfIsStringViewArgs = typename std::enable_if_t<
			std::is_constructible_v<std::string_view, Args...>, bool>;
	}

	/**
	* @brief	NtStringView provides a thin, type-safe wrapper for a null-terminated std::string_view
				
		* Can be constructed from most standard c++/c null-terminated string representations, similary to std::string_view
		* Will explicitly add the NULL byte to the underlying std::string_view 
		* Provides constexpr support
		* Attempting to construct using a non-null-terminated char array may result in undefined behavior!
		(some protection is provided, but buffer overflow cannot be detected at run time)
		
	*/
	class NtStringView
	{
	public:
		constexpr static inline bool IsExplicitNt(std::string_view sv);
		

		template <std::size_t N>
		constexpr explicit NtStringView(const char(&literal)[N]);

		template <typename... Args, detail::EnableIfIsStringViewArgs<Args...> = true>
		constexpr explicit NtStringView(Args&&... args);

		constexpr inline const char* NullTerminatedCStr() const noexcept;
		

		constexpr inline operator std::string_view() const noexcept;
		

		constexpr inline std::string_view ViewApi() const noexcept;
		
		
	private:
		std::string_view sv_;
	};

	constexpr inline bool NtStringView::IsExplicitNt(std::string_view sv)
	{
		return '\0' == sv.back();
	}

	template <std::size_t N>
	constexpr NtStringView::NtStringView(const char(&literal)[N])
		: sv_(literal, N)
	{
		if (!IsExplicitNt(sv_))
			throw std::runtime_error("Failed to verify string_view is null terminated - unsafe behavior NOT supported!");
	}

	template <typename... Args, detail::EnableIfIsStringViewArgs<Args...>>
	constexpr NtStringView::NtStringView(Args&&... args)
		: sv_(std::forward<Args>(args)...)
	{
		if (!IsExplicitNt(sv_))
		{
			sv_ = std::string_view(sv_.begin(), sv_.size() + 1);
			if (!IsExplicitNt(sv_))
				throw std::runtime_error("Failed to verify string_view is null terminated - unsafe behavior NOT supported!");
		}
	}

	template <>
	constexpr NtStringView::NtStringView(std::string&&) = delete;

	constexpr inline const char* NtStringView::NullTerminatedCStr() const noexcept
	{
		return sv_.data();
	}

	constexpr inline NtStringView::operator std::string_view() const noexcept
	{
		return sv_;
	}

	constexpr inline std::string_view NtStringView::ViewApi() const noexcept
	{
		return sv_;
	}


	
	
	//class UserEvent
	//{
	//public:
	//	/**
	//	* @brief	Constructs UserEvent by wrapping user's callback.
	//	* @note		After construction, event should be registered in DDSParticipantImpl::RegisterOnRaisedEvent.
	//	* @param	callback Function to be called when Event is raised. 
	//	*/
	//	UserEvent(std::function<void(void)> callback);
	//		
	//	/**
	//	* @brief	Notifies the dispatching context (thread_pool) to execute this (previously registerd) event's callback
	//	*/
	//	void RaiseEvent();

	//private:
	//	std::function<void()>					callback_;
	//	dds::core::cond::GuardCondition			cond_;

	//	friend DDSParticipantImpl;
	//};

	//
	//inline UserEvent::UserEvent(std::function<void(void)> callback)
	//	: callback_(callback)
	//	, cond_()
	//{
	//	cond_.handler([this]()
	//		{
	//			cond_.trigger_value(false);
	//			callback_();
	//		});
	//}

	//inline void UserEvent::RaiseEvent()
	//{
	//	cond_.trigger_value(true);
	//}

}	// namespace dds_wrapper
#endif /* SAFER_STRING_VIEW_H */
