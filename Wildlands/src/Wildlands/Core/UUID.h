#pragma once

namespace Wildlands
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
}

namespace std
{
	template <typename T> struct hash;

	template<>
	struct hash<Wildlands::UUID>
	{
		std::size_t operator()(const Wildlands::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};
}


