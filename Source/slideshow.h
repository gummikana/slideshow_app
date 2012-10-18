#ifndef INC_SLIDESHOW_H
#define INC_SLIDESHOW_H

#include <map>
#include <vector>
#include <string>

#include "include_poro.h"

namespace as { class TextSprite; class Sprite; }

class SlideShowApp : public poro::DefaultApplication
{
public:

    SlideShowApp();

    void Init();

    void Draw( poro::IGraphics* graphics );

    void Update( float dt );

	void NextSlide();
	void PrevSlide();
	void CreateSlide();

	as::Sprite* CreateASlide( const std::string& text, const std::string& image_file );
	// ---

	virtual void MouseButtonDown(const poro::types::vec2& pos, int button) { if( button == poro::Mouse::MOUSE_BUTTON_LEFT ) NextSlide(); else if( button == poro::Mouse::MOUSE_BUTTON_RIGHT ) PrevSlide(); } 
	virtual void MouseButtonUp(const poro::types::vec2& pos, int button) { }

	virtual void OnKeyDown( int key, poro::types::charset unicode );
	virtual void OnKeyUp( int key, poro::types::charset unicode );

	// ---

	as::Sprite* mContainer;
	as::Sprite*	mCurrentSlide;

	std::vector< std::pair< std::string, std::string > > mSlides;
	int mSlidePosition;

	// ---

	bool mSaveSlides;

};

#endif
