
vergeclass 'BoxStyle' do
    function BoxStyle:__init() end
    function BoxStyle:DrawBox(x, y, x2, y2, dest) end
end

vergeclass 'MolassesBoxStyle'(BoxStyle) do
    local WINDOW_GRADIENT_STYLE_TOTAL = 6
    local WINDOW_GRADIENT_SOLID = 0
    local WINDOW_GRADIENT_SOLID2 = 1
    local WINDOW_GRADIENT_HORIZONTAL = 2
    local WINDOW_GRADIENT_HORIZONTAL2 = 3
    local WINDOW_GRADIENT_VERTICAL = 4
    local WINDOW_GRADIENT_VERTICAL2 = 5

    local WINDOW_GRADIENT_STYLE = WINDOW_GRADIENT_SOLID;

    local WINDOW_COLOR_FILL = vx.RGB(0x06, 0x40, 0x55) --vx.RGB(0xE, 0xE, 0x12);
    local WINDOW_COLOR_FILL2 = vx.RGB(0x84, 0x38, 0x9C)
    local WINDOW_COLOR_OUTLINE = vx.RGB(0x00, 0xFF, 0xFF) --vx.RGB(0xFB, 0xD9, 0xB3);

    local CURSOR_CORNER_SIZE = 4;
    local CURSOR_COLOR_FILL = vx.RGB(255, 204, 102);
    local CURSOR_COLOR_OUTLINE = 0;

    function MolassesBoxStyle:__init()
        super()
        -- Public margin for text positioning.
        self.text_margin = { x = 8, y = 8 }
    end
    
    function MolassesBoxStyle:DrawBox(x1, y1, x2, y2, dest)
        if WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_SOLID then
            dest:RectFill(x1 + 3, y1 + 3, x2 - 3, y2 - 3, WINDOW_COLOR_FILL)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_SOLID2 then
            dest:RectFill(x1 + 3, y1 + 3, x2 - 3, y2 - 3, WINDOW_COLOR_FILL2)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_HORIZONTAL then
            dest:RectHGrad(x1 + 3, y1 + 3, x2 - 3, y2 - 3, WINDOW_COLOR_FILL, WINDOW_COLOR_FILL2)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_HORIZONTAL2 then
            dest:RectHGrad(x1 + 3, y1 + 3, x2 - 3, y2 - 3, WINDOW_COLOR_FILL2, WINDOW_COLOR_FILL)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_VERTICAL then
            dest:RectVGrad(x1 + 3, y1 + 3, x2 - 3, y2 - 3, WINDOW_COLOR_FILL, WINDOW_COLOR_FILL2)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_VERTICAL2 then
            dest:RectVGrad(x1 + 3, y1 + 3, x2 - 3, y2 - 3, WINDOW_COLOR_FILL2, WINDOW_COLOR_FILL)
        end

        -- Elliptical effect and outlines colored here.

        dest:Line(x1, y1 + 3, x1, y2 - 3, WINDOW_COLOR_OUTLINE)
        if WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_VERTICAL then
            dest:RectVGrad(x1 + 1, y1 + 2, x1 + 1, y2 - 2, WINDOW_COLOR_FILL, WINDOW_COLOR_FILL2)
            dest:RectVGrad(x1 + 2, y1 + 1, x1 + 2, y2 - 1, WINDOW_COLOR_FILL, WINDOW_COLOR_FILL2)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_VERTICAL2 then
            dest:RectVGrad(x1 + 1, y1 + 2, x1 + 1, y2 - 2, WINDOW_COLOR_FILL2, WINDOW_COLOR_FILL)
            dest:RectVGrad(x1 + 2, y1 + 1, x1 + 2, y2 - 1, WINDOW_COLOR_FILL2, WINDOW_COLOR_FILL)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_SOLID2
            or WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_HORIZONTAL2 then
                dest:Line(x1 + 1, y1 + 2, x1 + 1, y2 - 2, WINDOW_COLOR_FILL2)
                dest:Line(x1 + 2, y1 + 1, x1 + 2, y2 - 1, WINDOW_COLOR_FILL2)
        else
            dest:Line(x1 + 1, y1 + 2, x1 + 1, y2 - 2, WINDOW_COLOR_FILL)
            dest:Line(x1 + 2, y1 + 1, x1 + 2, y2 - 1, WINDOW_COLOR_FILL)
        end


        dest:Line(x2, y1 + 3, x2, y2 - 3, WINDOW_COLOR_OUTLINE)
        if WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_VERTICAL then
            dest:RectVGrad(x2 - 1, y1 + 2, x2 - 1, y2 - 2, WINDOW_COLOR_FILL, WINDOW_COLOR_FILL2)
            dest:RectVGrad(x2 - 2, y1 + 1, x2 - 2, y2 - 1, WINDOW_COLOR_FILL, WINDOW_COLOR_FILL2)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_VERTICAL2 then
            dest:RectVGrad(x2 - 1, y1 + 2, x2 - 1, y2 - 2, WINDOW_COLOR_FILL2, WINDOW_COLOR_FILL)
            dest:RectVGrad(x2 - 2, y1 + 1, x2 - 2, y2 - 1, WINDOW_COLOR_FILL2, WINDOW_COLOR_FILL)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_SOLID
            or WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_HORIZONTAL2 then
                dest:Line(x2 - 1, y1 + 2, x2 - 1, y2 - 2, WINDOW_COLOR_FILL)  
                dest:Line(x2 - 2, y1 + 1, x2 - 2, y2 - 1, WINDOW_COLOR_FILL)
        else
            dest:Line(x2 - 1, y1 + 2, x2 - 1, y2 - 2, WINDOW_COLOR_FILL2)
            dest:Line(x2 - 2, y1 + 1, x2 - 2, y2 - 1, WINDOW_COLOR_FILL2)
        end


        dest:Line(x1 + 3, y1, x2 - 3, y1, WINDOW_COLOR_OUTLINE)
        if WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_HORIZONTAL then
            dest:RectHGrad(x1 + 2, y1 + 1, x2 - 2, y1 + 1, WINDOW_COLOR_FILL, WINDOW_COLOR_FILL2)
            dest:RectHGrad(x1 + 3, y1 + 2, x2 - 3, y1 + 2, WINDOW_COLOR_FILL, WINDOW_COLOR_FILL2)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_HORIZONTAL2 then
            dest:RectHGrad(x1 + 2, y1 + 1, x2 - 2, y1 + 1, WINDOW_COLOR_FILL2, WINDOW_COLOR_FILL)
            dest:RectHGrad(x1 + 3, y1 + 2, x2 - 3, y1 + 2, WINDOW_COLOR_FILL2, WINDOW_COLOR_FILL)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_SOLID2
            or WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_VERTICAL2 then
                dest:Line(x1 + 2, y1 + 1, x2 - 2, y1 + 1, WINDOW_COLOR_FILL2)
                dest:Line(x1 + 3, y1 + 2, x2 - 3, y1 + 2, WINDOW_COLOR_FILL2)
        else
            dest:Line(x1 + 2, y1 + 1, x2 - 2, y1 + 1, WINDOW_COLOR_FILL)
            dest:Line(x1 + 3, y1 + 2, x2 - 3, y1 + 2, WINDOW_COLOR_FILL)
        end

        dest:Line(x1 + 3, y2, x2 - 3, y2, WINDOW_COLOR_OUTLINE)
        if WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_HORIZONTAL then
            dest:RectHGrad(x1 + 2, y2 - 1, x2 - 2, y2 - 1, WINDOW_COLOR_FILL, WINDOW_COLOR_FILL2)
            dest:RectHGrad(x1 + 3, y2 - 2, x2 - 3, y2 - 2, WINDOW_COLOR_FILL, WINDOW_COLOR_FILL2)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_HORIZONTAL2 then
            dest:RectHGrad(x1 + 2, y2 - 1, x2 - 2, y2 - 1, WINDOW_COLOR_FILL2, WINDOW_COLOR_FILL)
            dest:RectHGrad(x1 + 3, y2 - 2, x2 - 3, y2 - 2, WINDOW_COLOR_FILL2, WINDOW_COLOR_FILL)
        elseif WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_SOLID
            or WINDOW_GRADIENT_STYLE == WINDOW_GRADIENT_VERTICAL2 then
                dest:Line(x1 + 2, y2 - 1, x2 - 2, y2 - 1, WINDOW_COLOR_FILL)
                dest:Line(x1 + 3, y2 - 2, x2 - 3, y2 - 2, WINDOW_COLOR_FILL)
        else
            dest:Line(x1 + 2, y2 - 1, x2 - 2, y2 - 1, WINDOW_COLOR_FILL2)
            dest:Line(x1 + 3, y2 - 2, x2 - 3, y2 - 2, WINDOW_COLOR_FILL2)
        end

        dest:SetPixel(x1 + 1, y1 + 2, WINDOW_COLOR_OUTLINE)
        dest:SetPixel(x1 + 2, y1 + 1, WINDOW_COLOR_OUTLINE)
        dest:SetPixel(x2 - 1, y1 + 2, WINDOW_COLOR_OUTLINE)
        dest:SetPixel(x2 - 2, y1 + 1, WINDOW_COLOR_OUTLINE)

        dest:SetPixel(x1 + 1, y2 - 2, WINDOW_COLOR_OUTLINE)
        dest:SetPixel(x1 + 2, y2 - 1, WINDOW_COLOR_OUTLINE)
        dest:SetPixel(x2 - 1, y2 - 2, WINDOW_COLOR_OUTLINE)
        dest:SetPixel(x2 - 2, y2 - 1, WINDOW_COLOR_OUTLINE)
    end
end

box_style = MolassesBoxStyle()
