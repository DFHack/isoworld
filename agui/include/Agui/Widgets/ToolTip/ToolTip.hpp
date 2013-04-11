/*   _____                           
 * /\  _  \                     __    
 * \ \ \_\ \      __    __  __ /\_\   
 *  \ \  __ \   /'_ `\ /\ \/\ \\/\ \  
 *   \ \ \/\ \ /\ \_\ \\ \ \_\ \\ \ \ 
 *    \ \_\ \_\\ \____ \\ \____/ \ \_\
 *     \/_/\/_/ \/____\ \\/___/   \/_/
 *                /\____/             
 *                \_/__/              
 *
 * Copyright (c) 2011 Joshua Larouche
 * 
 *
 * License: (BSD)
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Agui nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AGUI_TOOLTIP_HPP
#define AGUI_TOOLTIP_HPP

#include "Agui/Widget.hpp"
namespace agui {

	 /**
	 * Class that represents a ToolTip that can pop up when hovering over things.
	 *
	 * ActionEvent when:
	 *
	 * Clicked.
     * @author Joshua Larouche
     * @since 0.2.0
     */
	class AGUI_CORE_DECLSPEC ToolTip : public Widget {
	private:
		std::vector<std::string> wrappedText;
		AreaAlignmentEnum align;
		agui::Point preferredOffset;
		Widget* invoker;

	protected:
		ResizableText resizableText;
		virtual void paintComponent(const PaintEvent &paintEvent);
		virtual void paintBackground(const PaintEvent &paintEvent);
	public:
		ToolTip();
		const std::vector<std::string>& getAreaText() const;
		virtual void showToolTip(const std::string& text, int width, int x, int y, Widget* invoker);
		virtual void hideToolTip();
		virtual void mouseClick(MouseEvent &mouseEvent);
		virtual void setTextAlignment(AreaAlignmentEnum alignment);
		virtual AreaAlignmentEnum getTextAlignment() const;
		virtual void setPreferredOffset(Point offset);
		virtual Point getPreferredOffset() const;
		Widget* getInvoker() const;

	};
}
#endif
