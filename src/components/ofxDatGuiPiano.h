

#pragma once
#include "ofxDatGuiComponent.h"

class ofxDatGuiPianoKey : public ofxDatGuiInteractiveObject {

    public:

        ofxDatGuiPianoKey(int width, int height, int index)
        {
            kKeyPad = 1;
            baseWidth = width;
            baseHeight = height;
            kIndex = index;
            kSelected = false;
            isWhite = isWhiteKey(index);
            topPart = ofRectangle(0, 0, baseWidth, height / 2);
            bottomPart = ofRectangle(0, height / 2, baseWidth - (kKeyPad * 2), height / 2);

            if (isWhite) {
                //topPart.position.x = bottomPart.position.x = whiteIndex(index) * width;
                switch (index%12) {
                case 0:
                case 5:
                    topPart.width = (baseWidth * 0.75) - (2*kKeyPad);
                    topOffset = 0;
                    break;
                case 2:
                case 7:
                case 9:
                    topPart.width = (baseWidth * 0.5) - (2 * kKeyPad);
                    topOffset = baseWidth * 0.25;
                    break;
                case 4:
                case 11:
                    topPart.width = (baseWidth * 0.75) - (2 * kKeyPad);
                    topOffset = baseWidth * 0.25;
                    break;
                default:
                    ofLog(OF_LOG_ERROR, "white key where no white key should be");
                    break;
                }

            } else { // black key
                topOffset = width * -0.25 ;
                topPart.height -= kKeyPad;
                topPart.width = (baseWidth * 0.5) - (2 * kKeyPad);
                bottomPart.width = bottomPart.height = 0.0; // not drawn
            }
        }
    
        void setPosition(float x, float y)
        {
            origin.x = x;
            origin.y = y;
        }
    
        void draw(int x, int y)
        {
            topPart.x = x + origin.x + (whiteIndex(kIndex) * baseWidth) + topOffset + kKeyPad;
            topPart.y = y + origin.y;
            if (isWhite) {
                bottomPart.x = x + origin.x + whiteIndex(kIndex) * baseWidth + kKeyPad;
                bottomPart.y = y + origin.y + baseHeight/2;
            }
            ofPushStyle();
            ofFill();
            ofSetColor(kBkgdColor);
            ofDrawRectangle(topPart);
            if (isWhite) {
                ofDrawRectangle(bottomPart);
            }
             ofPopStyle();
        }
    
        void hitTest(ofPoint k, bool mouseDown)
        {
             if (inside(k) && !kSelected){
                 if (mouseDown){
                     kBkgdColor = colors.selected.button;
                 }   else{
                     kBkgdColor = colors.hover.button;
                 }
             }   else{
                 onMouseOut();
             }
        }

        bool inside(ofPoint k)
        {
        	// returns true if point is inside this key
            return topPart.inside(k) || bottomPart.inside(k);
        }
    
        void setSelected(bool selected)
        {
            kSelected = selected;
        }
    
        bool getSelected()
        {
            return kSelected;
        }
    
        void onMouseOut()
        {
            if (kSelected){
                kBkgdColor = colors.selected.button;
            }   else{
                kBkgdColor = colors.normal.button;
            }
        }
    
        void onMouseRelease(ofPoint m)
        {
            if (inside(m)) {
                kSelected = !kSelected;
                ofxDatGuiInternalEvent e(ofxDatGuiEventType::MATRIX_BUTTON_TOGGLED, kIndex);
                internalEventCallback(e);
            }
        }
    
        void setTheme(const ofxDatGuiTheme* theme)
        {
            kBkgdColor = theme->color.matrix.normal.button;
            colors.normal.label = theme->color.matrix.normal.label;
            colors.normal.button = theme->color.matrix.normal.button;
            colors.hover.label = theme->color.matrix.hover.label;
            colors.hover.button = theme->color.matrix.hover.button;
            colors.selected.label = theme->color.matrix.selected.label;
            colors.selected.button = theme->color.matrix.selected.button;
        }

    protected:

        bool isWhiteKey(int index)
        {
            vector<int> _BLACKKEYS = { 1,3,6,8,10 };
            for (auto key : _BLACKKEYS) {
                if (index % 12 == key) { return false; }
            }
            return true;
        }

        int whiteIndex(int index)
        {
            vector<int> _whiteIndex = {0,1,1,2,2,3,4,4,5,5,6,6};
            return _whiteIndex[index%12] + (index / 12)*7;
        }



    private:
        ofPoint origin;
		ofRectangle topPart;
		ofRectangle bottomPart;
        float topOffset;
        float baseWidth;
        float baseHeight;
        float kKeyPad;

		bool isWhite;
        int kIndex;
        bool kSelected;
        ofColor kBkgdColor;
        struct {
            struct{
                ofColor label;
                ofColor button;
            } normal;
            struct{
                ofColor label;
                ofColor button;
            } hover;
            struct{
                ofColor label;
                ofColor button;
            } selected;
        } colors;
};



class ofxDatGuiPiano : public ofxDatGuiComponent
{
    public:

// from matrix/////////////////////////////////////////////////////////////////////

        ofxDatGuiPiano(string label, int octaves = 1) : ofxDatGuiComponent(label)
        {
            kSingleNoteMode = true;
            kOctaves = octaves;
            mType = ofxDatGuiType::PIANO;	// TODO - make keyboard gui type
            setTheme(ofxDatGuiComponent::theme.get());
        }

        void setTheme(const ofxDatGuiTheme* theme)
        {
            setComponentStyle(theme);
            kFillColor = theme->color.inputAreaBackground;
            mStyle.stripe.color = theme->stripe.matrix;
            setWidth(theme->layout.width, theme->layout.labelWidth);
            attachKeys(theme);
        }


        void setWidth(int width, float labelWidth)
        {
            ofxDatGuiComponent::setWidth(width, labelWidth);
            kKeyboardRect.x = x;
            kKeyboardRect.y = y;
            kKeyboardRect.width = mStyle.width -mStyle.padding;


            mStyle.height = 30;
            kKeyboardRect.height = mStyle.height -(mStyle.padding * 2);
        }
    
        void setPosition(int x, int y)
        {
            ofxDatGuiComponent::setPosition(x, y);
            kKeyboardRect.x = x;
            kKeyboardRect.y = y +mStyle.padding;
        }
    
        bool hitTest(ofPoint m)
        {
            if (kKeyboardRect.inside(m)){
                for(int i=0; i<keys.size(); i++) keys[i].hitTest(m, mMouseDown);
                return true;
            }   else{
                for(int i=0; i<keys.size(); i++) keys[i].onMouseOut();
                return false;
            }
        }
    
        void draw()
        {
            if (!mVisible) return;
            ofPushStyle();
                ofxDatGuiComponent::draw();
                ofSetColor(kFillColor);
                ofDrawRectangle(kKeyboardRect);
                for(int i=0; i<keys.size(); i++) keys[i].draw(x+ mStyle.padding, y + mStyle.padding);
            ofPopStyle();
        }
    
        void clear()
        {
            for (int i=0; i<keys.size(); i++) keys[i].setSelected(false);
        }
    
        void setSelected(vector<int> v)
        {
            clear();
            for (int i=0; i<v.size(); i++) keys[v[i]].setSelected(true);
        }
    
        vector<int> getSelected()
        {
            vector<int> selected;
            for(int i=0; i<keys.size(); i++) if (keys[i].getSelected()) selected.push_back(i);
            return selected;
        }
    
        ofxDatGuiPianoKey* getChildAt(int index)
        {
            return &keys[index];
        }
    
        static ofxDatGuiPiano* getInstance() { return new ofxDatGuiPiano("X", 1); }
    
    protected:
    
        void onMouseRelease(ofPoint m)
        {
            ofxDatGuiComponent::onFocusLost();
            ofxDatGuiComponent::onMouseRelease(m);
            for(int i=0; i<keys.size(); i++) keys[i].onMouseRelease(m);
        }
    
        void onButtonSelected(ofxDatGuiInternalEvent e)
        {
            if (kSingleNoteMode) {
        // deselect all buttons save the one that was selected //
                for(int i=0; i<keys.size(); i++) keys[i].setSelected(e.index == i);
            }
            if (pianoEventCallback != nullptr) {
                ofxDatGuiPianoEvent ev(this, e.index, keys[e.index].getSelected());
                pianoEventCallback(ev);
            }   else{
                ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
            }
        }
   

		void attachKeys(const ofxDatGuiTheme* theme)
		{
			keys.clear();
            float height = kKeyboardRect.getHeight();
			float width = kKeyboardRect.getWidth()  / (7 * kOctaves);

            for (int i = 0; i < 12 * kOctaves; i++) {
                ofxDatGuiPianoKey key(width, height, i);
				key.setTheme(theme);
				key.onInternalEvent(this, &ofxDatGuiPiano::onButtonSelected);
				keys.push_back(key);
			}
            ofLog(OF_LOG_NOTICE, "done");
		}


    private:
        int kOctaves;
        bool kSingleNoteMode;
        ofColor kFillColor;
        ofRectangle kKeyboardRect;
        vector<ofxDatGuiPianoKey> keys;




};

