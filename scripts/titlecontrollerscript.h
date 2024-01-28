#pragma once
#include "../Core/scripting/ntshengn_script.h"

using namespace NtshEngn;
struct TitleControllerScript : public Script {
	NTSHENGN_SCRIPT(TitleControllerScript);

	void init() {
		titleSound = getSoundID(*loadSound("assets/sounds/clownshop.wav"));
		titleSoundSource = playSound(titleSound, 0.25f);
		font = getFontID(*loadFont("assets/fonts/JetBrainsMono-Regular.ttf", 32));
	}

	void update(double dt) {
		NTSHENGN_UNUSED(dt);
		UIElementState buttonState = drawUIButton(Math::vec2((float)(getWindowWidth() / 2 - 160), (float)(getWindowHeight() / 2 - 50)), Math::vec2(320.0f, 50.0f), Math::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		if (buttonState == UIElementState::Hovered) {
			drawUIText(font, "Enter the Clown Shop", Math::vec2((float)(getWindowWidth() / 2 - 150), (float)(getWindowHeight() / 2)), Math::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else if (buttonState == UIElementState::Pressed) {
			if (isSoundPlaying(titleSound)) {
				stopSoundSource(titleSoundSource);
			}
			goToScene("assets/scenes/scene.ntsn");
			return;
		}
		else {
			drawUIText(font, "Enter the Clown Shop", Math::vec2((float)(getWindowWidth() / 2 - 150), (float)(getWindowHeight() / 2)), Math::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}

	void destroy() {
	}

private:
	enum class UIElementState {
		None,
		Pressed,
		Held,
		Released,
		Hovered
	};

	UIElementState drawUIButton(const Math::vec2& position, const Math::vec2& size, const Math::vec4& color = Math::vec4(1.0f, 1.0f, 1.0f, 1.0f), InputMouseButton mouseButton = InputMouseButton::One) {
		if (!graphicsModule || !windowModule) {
			return UIElementState::None;
		}

		graphicsModule->drawUIRectangle(position, size, color);

		int cursorPositionX = windowModule->getCursorPositionX(windowModule->getMainWindowID());
		int cursorPositionY = windowModule->getCursorPositionY(windowModule->getMainWindowID());
		if ((cursorPositionX >= position.x) && (cursorPositionX <= (position.x + size.x)) &&
			(cursorPositionY >= position.y) && (cursorPositionY <= (position.y + size.y))) {
			if (getMouseButtonState(mouseButton) == InputState::Pressed) {
				return UIElementState::Pressed;
			}
			else if (getMouseButtonState(mouseButton) == InputState::Held) {
				return UIElementState::Held;
			}
			else if (getMouseButtonState(mouseButton) == InputState::Released) {
				return UIElementState::Released;
			}
			else {
				return UIElementState::Hovered;
			}
		}

		return UIElementState::None;
	}

private:
	SoundID titleSound;
	SoundSourceID titleSoundSource;
	FontID font;
};