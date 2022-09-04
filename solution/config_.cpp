#include "Hooks.h"
#include "config_.hpp"

item_setting* config::get_by_definition_index(const int definition_index)
{

	/*
	auto it = std::find_if(m_items.begin(), m_items.end(), [definition_index](const item_setting& e)
		{
			return e.enabled && e.definition_index == definition_index;
		});

	return it == m_items.end() ? nullptr : &*it;
	*/

	for (size_t i = 0; i < m_items.size(); i++)
	{
		auto& item = m_items.at(i);

		if (item.enabled && item.definition_index == definition_index)
			return &m_items.at(i);
	}
	return nullptr;

}