#pragma once
#include "ofxDatGuiComponent.h"


class ofxDatGuiSelectorButton : public ofxDatGuiInteractiveObject
{

public:

    ofxDatGuiSelectorButton(int size, int index, bool showLabels)
    {
        mIndex = index;
        mSelected = false;
        mShowLabels = showLabels;
        mRect = ofRectangle(0, 0, size, size);
    }

    void setPosition(float x, float y)
    {
        origin.x = x;
        origin.y = y;
    }

    void draw(int x, int y, string label)
    {
        mRect.x = x + origin.x;
        mRect.y = y + origin.y;
        ofPushStyle();
        ofFill();
        ofSetColor(mBkgdColor);
        ofDrawRectangle(mRect);
        if (mShowLabels) {
            ofSetColor(mLabelColor);
            mFont->draw(label,
                mRect.x + mRect.width / 2 - (((mFontRect.width + 1) * label.length()) / 2),
                mRect.y + mRect.height / 2 + mFontRect.height / 2);
        }
        ofPopStyle();
    }

    void hitTest(ofPoint m, bool mouseDown)
    {
        if (mRect.inside(m) && !mSelected) {
            if (mouseDown) {
                mBkgdColor = colors.selected.button;
                mLabelColor = colors.selected.label;
            } else {
                mBkgdColor = colors.hover.button;
                mLabelColor = colors.hover.label;
            }
        } else {
            onMouseOut();
        }
    }

    void setSelected(bool selected)
    {
        mSelected = selected;
    }

    bool getSelected()
    {
        return mSelected;
    }

    void onMouseOut()
    {
        if (mSelected) {
            mBkgdColor = colors.selected.button;
            mLabelColor = colors.selected.label;
        } else {
            mBkgdColor = colors.normal.button;
            mLabelColor = colors.normal.label;
        }
    }

    void onMouseRelease(ofPoint m)
    {
        if (mRect.inside(m)) {
            mSelected = !mSelected;
            ofxDatGuiInternalEvent e(ofxDatGuiEventType::MATRIX_BUTTON_TOGGLED, mIndex);
            internalEventCallback(e);
        }
    }

    void setTheme(const ofxDatGuiTheme* theme)
    {
        mFont = theme->font2.ptr;

        mFontRect = mFont->rect(ofToString(mIndex + 2));
        mBkgdColor = theme->color.matrix.normal.button;
        mLabelColor = theme->color.matrix.normal.label;
        colors.normal.label = theme->color.matrix.normal.label;
        colors.normal.button = theme->color.matrix.normal.button;
        colors.hover.label = theme->color.matrix.hover.label;
        colors.hover.button = theme->color.matrix.hover.button;
        colors.selected.label = theme->color.matrix.selected.label;
        colors.selected.button = theme->color.matrix.selected.button;
    }

private:
    int x;
    int y;
    int mIndex;
    ofPoint origin;
    ofRectangle mRect;
    ofColor mBkgdColor;
    ofColor mLabelColor;
    bool mSelected;
    bool mShowLabels;
    ofRectangle mFontRect;
    shared_ptr<ofxSmartFont> mFont;
    struct
    {
        struct
        {
            ofColor label;
            ofColor button;
        } normal;
        struct
        {
            ofColor label;
            ofColor button;
        } hover;
        struct
        {
            ofColor label;
            ofColor button;
        } selected;
    } colors;

};



class ofxDatGuiSelector : public ofxDatGuiComponent
{

public:



    ofxDatGuiSelector(string label) : ofxDatGuiComponent(label)
    {
        mRadioMode = false;
        optionSelected = 0;
        mType = ofxDatGuiType::SELECTOR;
        setTheme(ofxDatGuiComponent::theme.get());
    }

    void setTheme(const ofxDatGuiTheme* theme)
    {
        setComponentStyle(theme);
        mFont = theme->font.ptr;
        mFontRect = mFont->rect("1");

        mFillColor = theme->color.inputAreaBackground;
        mTextColor = theme->color.textInput.text;
        mButtonSize = theme->layout.matrix.buttonSize;
        mButtonPadding = theme->layout.matrix.buttonPadding;
        mStyle.stripe.color = theme->stripe.matrix;
        attachButtons(theme);
        setWidth(theme->layout.width, theme->layout.labelWidth);
    }

    void setWidth(int width, float labelWidth)
    {
        ofxDatGuiComponent::setWidth(width, labelWidth);
        mSelectorRect.x = x + mLabel.width;
        mSelectorRect.y = y + mStyle.padding;
        mSelectorRect.width = mStyle.width - mStyle.padding - mLabel.width;
        int nCols = 2;
        if (btns.size() == 2) {
            btns[0].setPosition(0, mStyle.padding);
            btns[1].setPosition((mButtonSize + mStyle.padding), mStyle.padding);
        }
        mStyle.height = (mStyle.padding * 2) + mButtonSize;
        mSelectorRect.height = mStyle.height - (mStyle.padding * 2);
    }

    void setPosition(int x, int y)
    {
        ofxDatGuiComponent::setPosition(x, y);
        mSelectorRect.x = x + mLabel.width;
        mSelectorRect.y = y + mStyle.padding;
    }

    bool hitTest(ofPoint m)
    {
        if (mSelectorRect.inside(m)) {
            for (int i = 0; i<btns.size(); i++) btns[i].hitTest(m, mMouseDown);
            return true;
        } else {
            for (int i = 0; i<btns.size(); i++) btns[i].onMouseOut();
            return false;
        }
    }

    void draw()
    {
        if (!mVisible) return;
        ofPushStyle();
        ofxDatGuiComponent::draw();
        ofSetColor(mFillColor);
        ofDrawRectangle(mSelectorRect);
        if (btns.size()==2) {
            btns[0].draw(x + mLabel.width, y, "-");
            btns[1].draw(x + mLabel.width, y, "+");
        }
        ofSetColor(mTextColor);

        mFont->draw(mOptions[optionSelected], x + mLabel.width +mButtonSize*2.5 - mFontRect.width/2,
            mSelectorRect.y + mSelectorRect.height / 2 + mFontRect.height / 2);


        ofPopStyle();
    }


    static ofxDatGuiSelector* getInstance() { return new ofxDatGuiSelector("X"); }



    void setOptions(vector<string> options)
    {
        mOptions.resize(options.size());
        vector<string>::size_type i = 0;
        while (i < mOptions.size() && i < options.size()) {
            mOptions[i] = options[i].substr(0, 3);
            i++;
        }
    }


protected:

    void onMouseRelease(ofPoint m)
    {
        ofxDatGuiComponent::onFocusLost();
        ofxDatGuiComponent::onMouseRelease(m);
        for (int i = 0; i<btns.size(); i++) btns[i].onMouseRelease(m);
    }

    void onButtonSelected(ofxDatGuiInternalEvent e)
    {
        // deselect all buttons - don't want select behaviour we're inheriting from button matrix //
        for (int i = 0; i<btns.size(); i++) btns[i].setSelected(false);
        changeOption(e.index);

        if (SelectorEventCallback != nullptr) {
            ofxDatGuiSelectorEvent ev(this, e.index, true);
            SelectorEventCallback(ev);
        } else {
            ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
        }
    }

    void attachButtons(const ofxDatGuiTheme* theme)
    {
        btns.clear();
        for (int i = 0; i < 2; i++) {
            ofxDatGuiSelectorButton btn(mButtonSize, i, mShowLabels);
            btn.setTheme(theme);
            btn.onInternalEvent(this, &ofxDatGuiSelector::onButtonSelected);
            btns.push_back(btn);
        }
    }

    void changeOption(int buttonIndex)
    {
        if (buttonIndex == 1) {
            // up
            if (optionSelected >= mOptions.size()-1) {
                optionSelected = mOptions.size()-1;
            } else {
                optionSelected++;
            }
        } else {
            // up
            if (optionSelected <= 0) {
                optionSelected = 0;
            } else {
                optionSelected--;
            }
        }
    }

private:

    int mButtonSize;
    int optionSelected;
    int mButtonPadding;
    bool mRadioMode;
    bool mShowLabels;

    ofRectangle mFontRect;
    shared_ptr<ofxSmartFont> mFont;

    vector<string> mOptions;
    ofColor mFillColor;
    ofColor mTextColor;
    ofRectangle mSelectorRect;
    vector<ofxDatGuiSelectorButton> btns;
    typedef std::function<void(ofxDatGuiSelectorEvent)> onSelectorEventCallback;
    onSelectorEventCallback SelectorEventCallback;
};

