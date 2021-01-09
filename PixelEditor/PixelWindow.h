#pragma once
#include "World.h"
#include "PixelMath.h"
#include <vector>

class PixelWindow
{
public:
	PixelWindow(PixelMath::Vec2D position, PixelMath::Vec2D size, olc::Pixel color = olc::DARK_GREY, std::string WindowName = "Pixel Window", bool TitleBar = true, olc::Pixel TitleBarColor = { 45, 45, 46, 255 });
	~PixelWindow();

	enum AnchorOptions
	{
		TopLeft, TopCenter, TopRight, MiddleLeft, Center, MiddleRight,
		BottomLeft, BottomCenter, BottomRight, None
	};


private:
	struct AnchorOffset
	{
		float TopLeft = 0.0f, TopCenter = 0.0f, TopRight = 0.0f, MiddleLeft = 0.0f, Center = 0.0f, MiddleRight = 0.0f,
			BottomLeft = 0.0f, BottomCenter = 0.0f, BottomRight = 0.0f;
	};

	AnchorOffset availableAnchorOffsets;
	float anchorOffset = 10.0f;

	//olc::PixelGameEngine* engine = nullptr;
	PixelMath::Vec2D pw_position = {100, 100};
	PixelMath::Vec2D pw_size = {110, 250};
	olc::Pixel pw_back_color = olc::VERY_DARK_GREY;
	PixelMath::Vec2D offset = {0, 0};
	PixelMath::Vec2D pw_initial_size = { 110, 250 };
	std::string windowName = "Pixel Window";
	inline void AddTitleBar();
	bool bTitleBar = false;
	olc::Pixel pw_title_bar_color = { 45, 45, 46, 255 };
	//void CollapseAndExpandWindow();
	//bool bIsCollapsed = false;

	//Dragging
	bool CanDrag = true;
	bool LockDragging = false;
	bool isObjectDraggedByMouse = false;

	bool isBeingResized = false;
	bool bCanResize = true;
	bool isLMB_Pressed  = false; // to remove the magnet effect
	PixelMath::Vec2D MouseInitialResizePos = { 0,0 };

public:
	int NextAvailableID = 0;
	class PWChild
	{
	friend class PixelWindow;
	public:
		PWChild() {};
		virtual ~PWChild() {};
		
		void SetPosition(PixelMath::Vec2D pos) { position = pos; }
		void SetPosition(int posX, int posY) { position = { (float)posX, (float)posY }; }
		void SetPositionX(float posX) { position.X = posX; }
		void SetPositionY(float posY) { position.Y = posY; }
		PixelMath::Vec2D GetPosition() { return position; }
		PixelMath::Vec2D GetOffset() { return child_offset; }
		void SetSize(PixelMath::Vec2D newSize) { size = newSize; }
		PixelMath::Vec2D GetSize() { return size; }
		void SetAnchor(AnchorOptions option) { anchor = option; };
		void SetTextScale(float scale) { textScale = scale; }
		float GetTextScale() { return textScale; }
		void IgnoreScrolling(bool ignore) { bIgnoreScrolling = ignore; }

	private:
		// PreUpdate will always be active. It's used for pre-calulations that will determine if Update function will execute
		virtual void PreUpdate() = 0;
		// Used mainly for drawing (will only be active if the child is within the boundries of the window)
		virtual void Update() = 0;
		AnchorOptions anchor = AnchorOptions::None;
		PixelMath::Vec2D position = { 0, 0 };
		PixelMath::Vec2D size = { 10, 10 };
		PixelMath::Vec2D child_offset = { 0, 0 };
		float textScale = 1;
		bool bIgnoreScrolling = false;

	protected:
		unsigned int ID = 0;
		std::string typeName = "PWChild";
	};

	std::vector<PWChild*> children;

	class Separator : public PWChild //////////////////////////////////////////////////////
	{
	public:
		Separator(PixelMath::Vec2D* windowPosition, PixelMath::Vec2D* windowSize, float OffsetY, float sizeY, olc::Pixel co)
		{
			position.X = windowPosition->X;
			position.Y = windowPosition->Y + OffsetY;
			size.X = windowSize->X;
			size.Y = sizeY;
			color = co;
			Offset = OffsetY;
			winSize = windowSize;
			winPos = windowPosition;
			typeName = "Separator | ID: " + std::to_string(ID);
		}
		~Separator() {};

		void ChangeColor(olc::Pixel newColor) { color = newColor; }

	private:
		float Offset;
		olc::Pixel color = {45, 45, 45, 255};
		PixelMath::Vec2D* winSize = nullptr;
		PixelMath::Vec2D* winPos = nullptr;

	private:
		void PreUpdate() override
		{
			size.X = winSize->X;
			child_offset.Y = Offset;
			child_offset.X = 0.0f;
		}
		

		void Update() override
		{
			engine->FillRect(position.X, position.Y, size.X, size.Y, color);
		}
	
	};

	///////////// Button /////////////
	template <class t>
	class Button : public PWChild //////////////////////////////////////////////////////
	{
	public:
		Button(t* Owner, void(t::*funcToCall)(), PixelMath::Vec2D b_pos, PixelMath::Vec2D b_size, PixelMath::Vec2D b_offset, unsigned int id, std::string txt, olc::Pixel col)
		{
			position = b_pos;
			size = b_size;
			child_offset = b_offset;
			ID = id;
			name = "Button" + std::to_string(id);
			text = txt;
			color = col;
			hoveredOverColor = col;
			func = funcToCall;
			owner = Owner;
			typeName = "Button | ID: " + std::to_string(ID);
		}

		Button(t* Owner, std::function<void()>lambdaFuncToCall, PixelMath::Vec2D b_pos, PixelMath::Vec2D b_size, PixelMath::Vec2D b_offset, unsigned int id, std::string txt, olc::Pixel col)
		{
			position = b_pos;
			size = b_size;
			child_offset = b_offset;
			ID = id;
			name = "Button" + std::to_string(id);
			text = txt;
			color = col;
			hoveredOverColor = col;
			func = nullptr;
			lambdaFunc = lambdaFuncToCall;
			owner = Owner;
			typeName = "Button | ID: " + std::to_string(ID);
		}

		~Button() {};
		virtual void OnClickedEvent() { func ? ((*owner).*(func))() : lambdaFunc(); }
		void SetText(std::string txt) { text = txt; }
		void SetTextColor(olc::Pixel color) { textColor = color; }
		void SetButtonColor(olc::Pixel newColor) { color = newColor; }
		void SetHoveredOverColor(olc::Pixel newColor) { hoveredOverColor = newColor; }

	private:
		void PreUpdate() override
		{
		}

		void Update() override
		{
			//Button render
			if(isHoveredOver)
				engine->FillRect(position.X, position.Y, size.X, size.Y, hoveredOverColor);

			else
				engine->FillRect(position.X, position.Y, size.X, size.Y, color);

			//Text render
			float textYOffset = (size.Y / 2.0f) - 4.0f * textScale;
			float textXOffset = (size.X / 2.0f) - (text.length() * 4.0f * textScale);
			engine->DrawString(position.X + textXOffset, position.Y + textYOffset, text, textColor, textScale);

			if (position.X < engine->GetMouseX() && position.X + size.X > engine->GetMouseX() &&
				position.Y < engine->GetMouseY() && position.Y + size.Y > engine->GetMouseY())
			{
				isHoveredOver = true;
			}

			else
			{
				isHoveredOver = false;
			}

			if(engine->GetMouse(0).bPressed)
			{
				if(isHoveredOver)
				{
					OnClickedEvent();
				}
			}
		}

		std::string text = "BUTTON";
		std::string name;
		olc::Pixel color = olc::GREEN;
		olc::Pixel hoveredOverColor = olc::GREEN;
		olc::Pixel textColor = olc::YELLOW;
		void(t::*func)() = nullptr;
		std::function<void()>lambdaFunc = nullptr;
		t* owner = nullptr;
		bool isHoveredOver = false;
	};
	//////////////////////////

	class TickBox : public PWChild //////////////////////////////////////////////////////
	{
	public:
		TickBox(bool *value, PixelWindow* parentWindow, PixelMath::Vec2D box_position);
		~TickBox() { ; }

	private:
		bool *isTicked = nullptr;
		PixelWindow* myParentWindow;
		PixelWindow::Button<PixelWindow::TickBox>* myButton = nullptr;

		// Flip/Reverse the selection when pressed
		void onClicked()
		{
			if(isTicked)
			{
				if (*isTicked)
				{
					*isTicked = false;
					myButton->SetText("F");
					//myButton->SetButtonColor(olc::RED);
					//myButton->SetHoveredOverColor(olc::DARK_RED);
				}
				else
				{
					*isTicked = true;
					myButton->SetText("T");
					//myButton->SetButtonColor(olc::GREEN);
					//myButton->SetButtonColor(olc::DARK_GREEN);
				}
			}
		}

		virtual void PreUpdate() override
		{
		};

		virtual void Update() override
		{
		};
	};

	class Image : public PWChild //////////////////////////////////////////////////////
	{
	public:
		 Image(PixelMath::Vec2D ImagePosition, PixelMath::Vec2D ImageOffset, PixelMath::Vec2D ImageSize, olc::Sprite* image)
		 {
			 position = ImagePosition;
			 child_offset = ImageOffset;
			 scale.X = ImageSize.X / image->width;
			 scale.Y = ImageSize.Y / image->height;
			 size = ImageSize;
			 dImage = new olc::Decal(image);
			 dBackgroundImage = new olc::Decal(new olc::Sprite("..//PixelEditor//Sprites//Box.png"));
			 typeName = "Image | ID: " + std::to_string(ID);
		 };
		~Image(){};

		void SetImage(olc::Decal* image)
		{
			dImage = image; 
			scale.X = size.X / image->sprite->width;
			scale.Y = size.Y / image->sprite->height;
		}

	private:
		void PreUpdate() override
		{
		}


		void Update() override
		{
			engine->DrawDecal({ position.X, position.Y }, dBackgroundImage, { size.X + 10.0f, size.Y + 10.0f }, BackgroundColor);
			engine->DrawPartialDecal({ position.X + 5.0f, position.Y + 5.0f }, dImage, { 0.0f, 0.0f }, { size.X, size.Y });
		}

	private:
		olc::Decal* dBackgroundImage = nullptr;
		olc::Decal* dImage = nullptr;
		PixelMath::Vec2D scale;
		olc::Pixel BackgroundColor = {56, 58, 62, 255};
	};

	class Text : public PWChild //////////////////////////////////////////////////////
	{
	public:
		 Text(PixelMath::Vec2D TextPosition, PixelMath::Vec2D TextOffset, std::string txt, olc::Pixel TextColor)
		 {
			 position = TextPosition;
			 child_offset = TextOffset;
			 text = txt;
			 textColor = TextColor;
			 typeName = "Text | ID: " + std::to_string(ID);
		 };

		~Text() {};

	private:
		std::string text;
		olc::Pixel textColor;
		bool bCenterText = false;
		unsigned int lines = 1;

		void PreUpdate() override
		{
			size.Y = textScale * lines * 10.f;
		}

		void Update() override
		{
			if(bCenterText)
				engine->DrawString(position.X - (size.X), position.Y, text, textColor, textScale);

			else
				engine->DrawString( position.X, position.Y, text, textColor, textScale);
		}

	public:
		void SetText(std::string TEXT) { text = TEXT; }
		void CenterText(bool center) { bCenterText = center; }
		void AddNewLine() { text += "\n\n"; lines++; }
		void ClearTextAndLines() { text = ""; lines = 1; }
	};

	class Slider : public PWChild //////////////////////////////////////////////////////
	{
	public:
		Slider(PixelMath::Vec2D _pos, PixelMath::Vec2D _offset, PixelMath::Vec2D _size, float& _value, float _min, float _max)
		{
			position = _pos;
			size = _size;
			SliderValue = &_value;
			SliderMin = _min;
			SliderMax = _max;
			child_offset = _offset;

			SliderX = ((*SliderValue * 100.0f) / (SliderMax - SliderMin)) + SliderMin;
			SliderPosition.X = SliderX;
			SliderPosition.Y = position.Y - (size.Y * 5.0f);

			MousePointerOutOfBoundsValue = 20.0f * size.X;
			typeName = "Slider | ID: " + std::to_string(ID);
		};

		~Slider() {};
	
		bool* LMB_Pressed = nullptr;
		bool* allowWindowDrag = nullptr;
		bool* isBeingResized = nullptr;

	private:
		void PreUpdate() override
		{
			SliderPosition.X = position.X + SliderX;
			SliderPosition.Y = position.Y - (size.Y * 5.0f);
		}
	
		void Update() override
		{
			SliderFunctionality();
			isBeingDragged ? SliderColor = { 61, 255, 248, 255 } : SliderColor = olc::WHITE;
			engine->FillRect( position.X, position.Y , size.X * 100.0f, size.Y * 2.0f, olc::DARK_GREY );
			engine->FillRect(SliderPosition.X, SliderPosition.Y, size.X * 5.0f, size.Y * 10.0f, SliderColor);
		}
	
		PixelMath::Vec2D SliderPosition = { 5.0f, 0.0f };
		float* SliderValue = nullptr;
		float SliderMin = 0.0f;
		float SliderMax = 1.0f;
		void SliderFunctionality();
		bool isBeingDragged = false;
		float SliderX = 0.0f;
		float MousePointerOutOfBoundsValue = 20.0f;
		PixelMath::Vec2D MouseInitialPos = { 0.0f, 0.0f };
		olc::Pixel SliderColor = olc::WHITE;
	};

private:
	class ScrollBar : public PWChild //////////////////////////////////////////////////////
	{
	public:
		 ScrollBar(PixelMath::Vec2D* pw_pos, PixelMath::Vec2D* pw_size, float* GlobalOffset, float* LastObjectPosY, bool* canWindowDrag, float* globalBarValue)
		 {
			 position = *pw_pos;
			 size = *pw_size;
			 PW_POS = pw_pos;
			 PW_SIZE = pw_size;
			 f_globalOffset = GlobalOffset;
			 f_lastObjectPosY = LastObjectPosY;
			 b_canWindowDrag = canWindowDrag;
			 BarValue = globalBarValue;
			 typeName = "ScrollBar | ID: " + std::to_string(ID);

			 PixelMath::Vec2D tempPos = position;
			 tempPos.Y -= 10.0f;
		 };

		~ScrollBar() {};

	private:
		void PreUpdate() override
		{
			position.X = PW_POS->X + PW_SIZE->X - 10.0f;
			position.Y = PW_POS->Y + 30.0f;
			size.X = 10.0f;
			size.Y = PW_SIZE->Y - 60.0f;
		}


		void Update() override
		{
			PreUpdate();
			
			engine->FillRect(position.X, position.Y, size.X, size.Y, olc::DARK_GREY);

			Buttons();
			ScrollBarFunctionality();
		}

		PixelMath::Vec2D* PW_POS  = nullptr;
		PixelMath::Vec2D* PW_SIZE = nullptr;
		PixelMath::Vec2D MouseStartPos = { 0.0f, 0.0f };
		bool barDragging = false;
		float* BarValue;
		float* f_globalOffset;
		float* f_lastObjectPosY;
		float* f_lastObjectOnYSize;
		bool* b_canWindowDrag;
		bool lockDragging = false;
		olc::Pixel ButtonUpColor = olc::GREY;
		olc::Pixel ButtonDownColor = olc::GREY;
		olc::Pixel BarColor = olc::WHITE;
		float grabPointOnBar = 0.0f;

		void ScrollUp(float divider)
		{
			if(*f_globalOffset < 0.0f)
			*f_globalOffset += 1.0f / divider;
		}

		void ScrollDown(float divider)
		{
			//if (*f_lastObjectPosY > 0.0f)
			*f_globalOffset -= 1.0f / divider;
		}

		void Buttons()
		{
			float buttonSize = 10.0f;
			PixelMath::Vec2D ButtonUpPos = {position.X, position.Y - buttonSize};
			PixelMath::Vec2D ButtonDownPos = { position.X, position.Y + size.Y };
			engine->FillRect(ButtonUpPos.X, ButtonUpPos.Y, buttonSize, buttonSize, ButtonUpColor);
			engine->FillRect(ButtonDownPos.X, ButtonDownPos.Y, buttonSize, buttonSize, ButtonDownColor);
			ButtonUpColor   = olc::GREY;
			ButtonDownColor = olc::GREY;
			
			if (engine->GetMouse(0).bHeld)
			{

				if (ButtonUpPos.X < engine->GetMouseX() && ButtonUpPos.X + size.X > engine->GetMouseX() &&
					ButtonUpPos.Y < engine->GetMouseY() && ButtonUpPos.Y + buttonSize > engine->GetMouseY())
				{
					ButtonUpColor = { 188, 255, 255, 255 };
					*b_canWindowDrag = false;
					ScrollUp(1.0f);
					*BarValue = BarPosBasedOnGO() * -1.0f;
				}

				if (ButtonDownPos.X < engine->GetMouseX() && ButtonDownPos.X + size.X > engine->GetMouseX() &&
					ButtonDownPos.Y < engine->GetMouseY() && ButtonDownPos.Y + buttonSize > engine->GetMouseY())
				{
					*b_canWindowDrag = false;
					ButtonDownColor = { 188, 255, 255, 255 };
					
					float MaxBarValue = size.Y - 50.0f;
					if(*BarValue < MaxBarValue)
					{
						ScrollDown(1.0f);
						*BarValue = BarPosBasedOnGO() * -1.0f;
					}
				}
			}
		}

		void ScrollBarFunctionality()
		{
			PixelMath::Vec2D BarPos = { position.X + 2.0f, position.Y + *BarValue };
			PixelMath::Vec2D BarSize = { size.X - 4.0f, 50.0f };
			engine->FillRect(BarPos.X, BarPos.Y, BarSize.X, BarSize.Y, BarColor);
			BarColor = olc::WHITE;

			if (engine->GetMouse(0).bHeld && !lockDragging)
			{
				if (BarPos.X < engine->GetMouseX() && BarPos.X + BarSize.X > engine->GetMouseX() &&
					BarPos.Y < engine->GetMouseY() && BarPos.Y + BarSize.Y > engine->GetMouseY() && !barDragging)
				{
					barDragging = true;
					BarColor = { 188, 255, 255, 255 };
					*b_canWindowDrag = false;
					MouseStartPos = { (float)engine->GetMouseX(), (float)engine->GetMouseY() };
					grabPointOnBar = engine->GetMouseY() - BarPos.Y;
				}

				if(barDragging)
				{
				   *b_canWindowDrag = false;
				   PixelMath::Vec2D MouseCurrentPos = { (float)engine->GetMouseX(), (float)engine->GetMouseY() };

				   BarColor = { 188, 255, 255, 255 };

				   PixelMath::Vec2D MouseOffset = MouseCurrentPos - BarPos;
				   MouseOffset.Y -= BarSize.Y / 2.0f;

				   float MaxOffset = 50.0f;
				   float MaxBarValue = size.Y - BarSize.Y;

				   // Stop dragging if the mouse pointer moves too far from the bar
				   if (MouseOffset.X > MaxOffset || MouseOffset.X < -MaxOffset
					   || MouseOffset.Y > MaxOffset || MouseOffset.Y < -MaxOffset)
					   barDragging = false;

						//float grabPointOnBar = MouseCurrentPos.Y - BarPos.Y;
						engine->DrawString(0.0f, 0.0f, std::to_string(grabPointOnBar));
				   if(MouseCurrentPos.Y < MouseStartPos.Y)
				   {
						MouseStartPos = { (float)engine->GetMouseX(), (float)engine->GetMouseY() };

						ScrollUp(MaxBarValue);

						*BarValue = MouseCurrentPos.Y - (grabPointOnBar) - (position.Y);
				   }

				   else if (MouseCurrentPos.Y > MouseStartPos.Y)
				   {
						MouseStartPos = { (float)engine->GetMouseX(), (float)engine->GetMouseY() };

						ScrollDown(MaxBarValue);
						*BarValue = MouseCurrentPos.Y - (grabPointOnBar) - (position.Y);
				   }

				  if (*BarValue < 0.0f) { *BarValue = 0.0f; barDragging = false; lockDragging = true; }
				  if (*BarValue > MaxBarValue + 1.0f) { *BarValue = MaxBarValue; barDragging = false; lockDragging = true; }

				  *f_globalOffset = GO_BasedOnBar() * -1.0f; // reverse global offset

				}
			}


			if (engine->GetMouse(0).bReleased)
			{
				barDragging = false;
				lockDragging = false;
				grabPointOnBar = 0.0f;
			}
		}

		//Calculate Global Offset Based On The Bar Position
		float GO_BasedOnBar()
		{
			float result = 0.0f;
			float BarSize = 50.0f;
			float MaxBarValue = size.Y - BarSize;
			float MaxGlobalOffset = *f_lastObjectPosY + 20.0f;// + (PW_SIZE->Y / 2.0f); add this to move the last object to the half of the window
			float BarCurrentValue = *BarValue;

			result = (MaxGlobalOffset * BarCurrentValue) / MaxBarValue;
			
			return result;
		}

		float BarPosBasedOnGO()
		{
			float result = 0.0f;
			float BarSize = 50.0f;
			float MaxBarValue = size.Y - BarSize;
			float MaxGlobalOffset = *f_lastObjectPosY + 20.0f;
			float CurrentGlobalOffset = *f_globalOffset;
		
			result = (CurrentGlobalOffset * MaxBarValue) / MaxGlobalOffset;
		
			return result;
		}
	};
////////////////////////////////////////////////////////////////////

private:
	void MouseDrag();
	void Resize();
	bool bVisible = true;
	void AddChild(PWChild* child);

public:
	void Update();
	
	//Add button function that takes a function pointer as an argument
	template <class parent>
	Button<parent>* AddButton(parent* Parent, void(parent::*functionToCall)(), PixelMath::Vec2D position, PixelMath::Vec2D size, std::string text, olc::Pixel color);

	//Add button function that takes a lambda function as an argument
	template <class parent>
	Button<parent>* AddButton(parent* Parent, std::function<void()>lambdaFuncToCall, PixelMath::Vec2D position, PixelMath::Vec2D size, std::string text, olc::Pixel color);

	TickBox* AddTickBox(bool *bValue, PixelWindow* ParentWindow, PixelMath::Vec2D v2d_position);
	Separator* AddSeparator(float offsetY, olc::Pixel color = { 45, 45, 45, 255 });
	Image* AddImage(PixelMath::Vec2D imagePosition, PixelMath::Vec2D imageSize, olc::Sprite* Image);
	Text* AddText(PixelMath::Vec2D textPosition, std::string text, olc::Pixel textColor = olc::WHITE);
	Slider* AddSlider(PixelMath::Vec2D position, PixelMath::Vec2D scale, float& value, float min, float max);

	void SetWindowPosition(int x, int y);
	void SetWindowPosition(PixelMath::Vec2D position);
	void SetWindowSize(int width, int height) { pw_size = { (float)width, (float)height }; }
	void SetWindowSize(PixelMath::Vec2D size) { pw_size = size; }
	void SetTitleBarColor(olc::Pixel color) { pw_title_bar_color = color; titleBarBackground->ChangeColor(color); }
	void Show() { bVisible = true; }
	void Hide() { bVisible = false; }
	bool isVisible() { return bVisible; }
	void AnchorChild(PWChild* child);
	void ResetAnchorValues();
	PixelMath::Vec2D GetSize()		{ return pw_size; }
	PixelMath::Vec2D GetPosition() { return pw_position; }
	olc::Pixel GetBackgroundColor() { olc::Pixel tempCol = pw_back_color; return tempCol; }
	void SetBackgroundColor(olc::Pixel newColor) { pw_back_color = newColor; }
	void DisableDragging(bool disableDrag) { LockDragging = disableDrag; }
	void CanBeResized(bool canResize) { bCanResize = canResize; }
	void ForceAddScrollBar() { ForceAScrollBar = true; }
	void Clear();

private:
	float fGlobalOffset = 0.0f;
	float fGlobalBarValue = 0.0f;
	bool NeedsAScrollBar = false;
	bool ForceAScrollBar = false;
	PWChild* sb_ScrollBar = nullptr;
	float lastObjectOnY = 0.0f;
	PixelWindow::Separator* titleBarBackground = nullptr;
};

template<class parent>
inline PixelWindow::Button<parent>* PixelWindow::AddButton(parent* Parent, void(parent::*functionToCall)(), PixelMath::Vec2D position, PixelMath::Vec2D size, std::string text, olc::Pixel color)
{
	Button<parent>* newButton = new Button<parent>(Parent, functionToCall, pw_position + position, size, position, NextAvailableID, text, color);
	AddChild(newButton);
	return newButton;
}

template<class parent>
inline PixelWindow::Button<parent>* PixelWindow::AddButton(parent* Parent, std::function<void()>lambdaFuncToCall, PixelMath::Vec2D position, PixelMath::Vec2D size, std::string text, olc::Pixel color)
{
	Button<parent>* newButton = new Button<parent>(Parent, lambdaFuncToCall, pw_position + position, size, position, NextAvailableID, text, color);
	AddChild(newButton);
	return newButton;
}