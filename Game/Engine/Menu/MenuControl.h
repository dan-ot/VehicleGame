//
// MenuControl.h - Interface for value-bearing menu items (edit boxes, sliders)
//

#pragma once

#include "../Assets/AssetManager.h"
#include "../Render/Primitives.h"
#include "../Render/QuadRenderer.h"
#include "../State/InputCommands.h"
#include "../State/RawKeyboardFrame.h"
#include "MenuItem.h"

#include <any>
#include <optional>
#include <utility>

namespace Engine
{
	enum class MenuInputOutcome
	{
		Handled,
		Ignored,
	};

	class IMenuControl
	{
	public:
		virtual ~IMenuControl() = default;

		virtual void Update(double dt) = 0;
		virtual void Render(QuadRenderer& renderer, const AssetManager& assets, const RectF& rect) const = 0;
		virtual MenuInputOutcome HandleInput(const CommandFrame& commands, const RawKeyboardFrame& keyboard, std::optional<std::any>& change) = 0;
	};

	template <typename T>
	class MenuControl : public IMenuControl
	{
	public:
		using ValueType = T;

		explicit MenuControl(T value = T{}) : m_value(std::move(value)) {}

		const T& Value() const noexcept { return m_value; }
		void SetValue(T value) { m_value = std::move(value); }

		MenuInputOutcome HandleInput(const CommandFrame& commands, const RawKeyboardFrame& keyboard, std::optional<std::any>& change) final
		{
			T next = m_value;
			const MenuInputOutcome outcome = OnInput(commands, keyboard, next);
			if (!(next == m_value))
			{
				m_value = next;
				change = std::any{ m_value };
			}
			return outcome;
		}

	protected:
		virtual MenuInputOutcome OnInput(const CommandFrame& commands, const RawKeyboardFrame& keyboard, T& value) = 0;

	private:
		T m_value;
	};
}
