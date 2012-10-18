#include "slideshow.h"

#include <sdl.h>
#include <GL/glew.h>

#include <utils/debug.h>
#include <utils/string/string.h>
#include <utils/filesystem/filesystem.h>

#include <game_utils/tween/tween_utils.h>
#include <game_utils/actionscript/actionscript.h>


SlideShowApp::SlideShowApp() :
	mContainer( NULL ),
	mCurrentSlide( NULL ),
	mSlides(),
	mSlidePosition( 0 ),
	mSaveSlides( true )
{
}


void SlideShowApp::Init()
{
    poro::DefaultApplication::Init();

	mContainer = new as::Sprite;


	// load the file
	{	
		const std::string filename = "data/slides.txt";
		std::fstream file_input;
		std::string line;

		file_input.open(filename.c_str() , std::ios::in );

		if(!file_input.good())
			logger << "Unable to open file: " << filename.c_str() << "\n";

		while ( file_input.good() ) 
		{
			std::getline( file_input, line );
			std::vector< std::string > lines = ceng::Split( "|", line );
			if( lines.size() >= 2 ) 
				mSlides.push_back( std::make_pair( lines[ 0 ], lines[ 1 ] ) );
		}

		file_input.close();
	}

	CreateSlide();
}

// -----------

void SlideShowApp::CreateSlide()
{
	std::string title_text = mSlides[ this->mSlidePosition ].first;
	std::string image_file = mSlides[ this->mSlidePosition ].second;
	as::Sprite* new_slide = CreateASlide( title_text, image_file );

	// just create a new slide, no tweening
	if( mSaveSlides ) 
	{
		delete mCurrentSlide;
		mCurrentSlide = new_slide;
		mContainer->addChild( new_slide );
	}
	else
	{

		// tween the slides in and out
		if( mCurrentSlide ) {
			GTweenSpriteAlphaTo( mCurrentSlide, 0, 0.2f, ceng::easing::Cubic::easeOut );
			KillSpriteSlowly( mCurrentSlide, 0.5f );
			mCurrentSlide = NULL;
		}

		if( new_slide ) {
			mCurrentSlide = new_slide;
			mContainer->addChild( new_slide );
			
			new_slide->SetAlpha( 0 );
			GTween* t = GTweenSpriteAlphaTo( new_slide, 1.f, 0.2f, ceng::easing::Cubic::easeOut );
			t->SetDelay( 0.2f );
		}
	}

}


// -----------

as::Sprite* SlideShowApp::CreateASlide( const std::string& title_text, const std::string& image_file )
{
	as::Sprite* result = new as::Sprite;

	as::Sprite* image = as::LoadSprite( image_file );
	if( image ) 
	{
		if( image->GetTexture() == NULL ) {
			image = as::LoadSprite( ceng::GetParentPath( image_file ) + "/" + ceng::GetFilenameWithoutExtension( image_file ) + ".png" );
		}

		types::vector2 size = image->GetTextureSize();
		image->SetCenterOffset( 0.5f * size );
		image->MoveTo( 0.5f * types::vector2( 1024, 768 ) );

		types::vector2 scale( 1, 1 );
		if( size.x != 0 ) scale.x = 1024.f / size.x;
		if( size.y != 0 ) scale.y = 768.f / size.y;

		float temp_scale = 1;
		bool scale_over = true;
		if( scale_over ) temp_scale = ceng::math::Max( scale.x, scale.y );
		else temp_scale = ceng::math::Min( scale.x, scale.y );

		image->SetScale( temp_scale, temp_scale );

	}
	result->addChild( image );

	as::Sprite* text_container = new as::Sprite;

	as::TextSprite* text = as::LoadTextSprite(  "data/fonts/helveticaneueltstd-blkcn.xml" );
	text->SetText( title_text );
	text->SetColor( 251.f / 255.f, 251.f / 255.f, 253.f / 255.f );
	text->MoveTo( types::vector2( 0, -32 ) );

	types::rect black_rect = text->GetBounds();
	// black_rect.y += 34;
	black_rect.h -= ( 37 );

	black_rect.x -= 24;
	black_rect.w += 2.f * 24;

	black_rect.y -= 24;
	black_rect.h += 2.f * 24;

	as::Sprite* black_box = as::LoadSprite( "data/images/black_box.png" );
	black_box->SetSize( (int)black_rect.w, (int)black_rect.h );
	text->MoveBy( -types::vector2( (float)black_rect.x, (float)black_rect.y ) );
	text_container->addChild( black_box );
	text_container->addChild( text );

	// std::cout << "Black rect.w " << black_rect.w << " > " << 1024.f - ( 2.f * ( 1024.f / 18.f ) ) << std::endl;
	if( black_rect.w > 1024.f - ( 2.f * ( 1024.f / 18.f ) ) )
	{
		float tscale = ( 1024.f - ( 2.f * ( 1024.f / 18.f ) ) ) / black_rect.w;
		text_container->SetScale( tscale, tscale );
	}

	text_container->MoveTo( types::vector2( 1024.f / 18.f, ( 768.f / 12.f ) * 9.f ) );
	// text_container->SetRotation( 0.2f );
	
	result->addChild( text_container );
	return result;
}


void SlideShowApp::Draw( poro::IGraphics* graphics )
{
	as::DrawSprite( mContainer, graphics );

	if( mSaveSlides ) 
	{
		bool kill_after = false;
		if( mSlidePosition == (int)mSlides.size() - 1 ) kill_after = true;
		std::stringstream ss;
		ss << "output/slide_" << std::setw( 3 ) << std::setfill( '0' ) << mSlidePosition  << ".png";
		Poro()->GetGraphics()->SaveScreenshot( ss.str() );
		NextSlide();

		if( kill_after ) Poro()->Exit();
	}
}

void SlideShowApp::Update( float dt )
{
	UpdateGTweens( dt );

}

void SlideShowApp::NextSlide()
{
	mSlidePosition = ceng::math::Clamp( ++mSlidePosition, 0, (int)mSlides.size() - 1 );
	CreateSlide();
}

void SlideShowApp::PrevSlide()
{
	mSlidePosition = ceng::math::Clamp( --mSlidePosition, 0, (int)mSlides.size() - 1 );
	CreateSlide();
}

void SlideShowApp::OnKeyDown( int key, poro::types::charset unicode ) 
{ 
	if( key == SDLK_LEFT || key == SDLK_BACKSPACE ) 
		PrevSlide();
	if( key == SDLK_RIGHT || key == SDLK_SPACE ) 
		NextSlide();


	if( key == SDLK_ESCAPE ) Poro()->Exit();
}

void SlideShowApp::OnKeyUp( int key, poro::types::charset unicode ) 
{ 
}
