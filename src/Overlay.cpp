#include "Overlay.h"

std::string to_string(flt_t number, int_t precision) {
	std::ostringstream buf;
	buf << std::setprecision(precision) << number;
	return buf.str();
}

bool MouseInView(const canvas_t& canvas) {
	return canvas.getViewport(canvas.getView()).contains(sf::Mouse::getPosition(canvas));
}

Overlay::Overlay(font_t& font, geo_ptr& scales) :
	MenuBox(font),
	_scales(scales),
	_iteration(0),
	_max_iterations(DEFAULT_MAX_ITERATIONS)
{
	add_text_item((int_t)LabelIndex::TITLE);
	add_text_item((int_t)LabelIndex::MOUSE_X);
	add_text_item((int_t)LabelIndex::MOUSE_Y);
	add_text_item((int_t)LabelIndex::POWER);
	add_text_item((int_t)LabelIndex::MAGNIFICATION);
	add_text_item((int_t)LabelIndex::ITERATION);
	add_text_item((int_t)LabelIndex::ALGORITHM);
	add_text_item((int_t)LabelIndex::COLOR_SCHEME);
	add_text_item(INIT_X_POS, INIT_Y_POS + ((int_t)LabelIndex::RENDERING) * ITEM_HEIGHT + 15);
	add_text_item(INIT_X_POS, INIT_Y_POS + ((int_t)LabelIndex::NOTIFICATION) * ITEM_HEIGHT + 15);

	type(MANDELBROT, INIT_PAIR);
	power(DEFAULT_POWER);
	magnification(DEFAULT_MAGNIFICATION);
	iteration(_iteration, _max_iterations);
	algorithm(DEFAULT_ALGORITHM_INDEX);
	color_scheme(DEFAULT_COLOR_SCHEME_INDEX);
}

int Overlay::update(canvas_t& window) {
	_mouse_x = sf::Mouse::getPosition(window).x;
	_mouse_y = sf::Mouse::getPosition(window).y;
	_labels[(int)LabelIndex::MOUSE_X].setString("X:  " + std::to_string(_scales->coord_x(_mouse_x)));
	_labels[(int)LabelIndex::MOUSE_Y].setString("Y:  " + std::to_string((-1) * _scales->coord_y(_mouse_y)));
	return (int)UpdateCodes::IDLE;
}

std::string Overlay::title() const {
	return _labels[(int)LabelIndex::TITLE].getString();
}

int_t Overlay::iteration() const {
	return _iteration;
}

int_t Overlay::init_iteration() {
	_iteration = 0;
	iteration(_iteration, _max_iterations);
	return _iteration;
}

int_t Overlay::next_iteration() {
	_iteration = _iteration + 1;
	iteration(_iteration, _max_iterations);
	return _iteration;
}

int_t Overlay::max_iterations() const {
	return _max_iterations;
}

std::string Overlay::notification() const {
	return _labels[(int)LabelIndex::NOTIFICATION].getString();
}

Geometry2D Overlay::scales() const {
	return *_scales;
}

Overlay& Overlay::type(int_t type, pair_t coords) {
	switch (type) {
	case MANDELBROT:
		init_title();
		break;
	case JULIA:
		julia_title(coords);
		break;
	};

	return *this;
}

Overlay& Overlay::title(const std::string& title) {
	_labels[(int_t)LabelIndex::TITLE].setString(title);
	return *this;
}

Overlay& Overlay::init_title() {
	title(DEFAULT_TITLE);
	return *this;
}

Overlay& Overlay::julia_title(pair_t coordinates) {
	title(
		"Julia Set (" + to_string(coordinates.first, DISPLAY_PRECISION)
		+ ", " + to_string(coordinates.secnd, DISPLAY_PRECISION) + ")"
	);
	return *this;
}

Overlay& Overlay::power(int_t value) {
	if (value > 0)
		_labels[(int)LabelIndex::POWER].setString("z^" + std::to_string(value) + " + c");
	else
		_labels[(int)LabelIndex::POWER].setString(GetFunctionName((-1)*value));

	return *this;
}

Overlay& Overlay::magnification(int_t value) {
	_labels[(int)LabelIndex::MAGNIFICATION]
		.setString(
			"Magnification:  "
			+ std::to_string((int_t)pow(TrackingBox::factor, value))
		);
	return *this;
}

Overlay& Overlay::iteration(int_t it) {
	_labels[(int)LabelIndex::ITERATION].setString("Iteration:  " + std::to_string(it) + " of " + std::to_string(_max_iterations));
	return *this;
}

Overlay& Overlay::iteration(int_t it, int_t max) {
	_max_iterations = max;
	return iteration(it);
}

Overlay& Overlay::algorithm(int_t value) {
	_labels[(int)LabelIndex::ALGORITHM].setString("Algorithm:  " + Overlay::GetAlgorithmName(value));
	return *this;
}

Overlay& Overlay::color_scheme(int_t value) {
	_labels[(int)LabelIndex::COLOR_SCHEME].setString("Color Scheme:  " + Overlay::GetColorSchemeName(value));
	return *this;
}

Overlay& Overlay::notification(const std::string& message) {
	_labels[(int)LabelIndex::NOTIFICATION].setString(message);
	return *this;
}

Overlay& Overlay::rendering(bool isRendering) {
	_labels[(int)LabelIndex::RENDERING].setString(isRendering ? "Rendering..." : "");
	return *this;
}

Overlay& Overlay::state(const State& other)
{
	type(other.type, other.j_coords);
	power(other.power);
	magnification(other.magnification);
	iteration(0, other.max_iterations);
	algorithm(other.algorithm_index);
	color_scheme(other.color_scheme_index);
	return *this;
}

std::string Overlay::GetFunctionName(int_t index) {
	switch ((Functions)(index - 1)) {
	case Functions::OPAL_VEIN:
		return "z * (sin(Re(z)), cos(Im(z))) + c";
	default:
		return "";
	}
}

std::string Overlay::GetAlgorithmName(int_t index) {
	switch ((Algorithms)index) {
	case Algorithms::ESCAPE_TIME:
		return "Escape Time";
	case Algorithms::POTENTIAL:
		return "Potential";
	default: return "";
	}
}

std::string Overlay::GetColorSchemeName(int_t index) {
	switch ((ColorSchemes)index) {
	case ColorSchemes::LINEAR:
		return "Linear";
	case ColorSchemes::HYPERBOLIC:
		return "Hyperbolic";
	case ColorSchemes::LOGARITHMIC:
		return "Logarithmic";
	case ColorSchemes::CIRCULAR:
		return "Circular";
	default: return "";
	}
}

int_t TrackingBox::factor = DEFAULT_ZOOM;

int_t TrackingBox::box_width = WIDTH_PIXELS / TrackingBox::factor;
int_t TrackingBox::box_height = HEIGHT_PIXELS / TrackingBox::factor;

int_t TrackingBox::box_x_radius = TrackingBox::box_width / 2;
int_t TrackingBox::box_y_radius = TrackingBox::box_height / 2;

TrackingBox::TrackingBox(canvas_t& canvas) :
	_shape(sf::Vector2f{ (float)TrackingBox::box_width, (float)TrackingBox::box_height })
{
	_shape.setPosition((flt_t)sf::Mouse::getPosition(canvas).x, (flt_t)sf::Mouse::getPosition(canvas).y);
	_shape.setOutlineColor(sf::Color(255, 255, 255, 128));
	_shape.setOutlineThickness(-3.f);
	_shape.setFillColor(sf::Color::Transparent);
}

void TrackingBox::move(int_t x, int_t y) {
	auto size = _shape.getSize();
	_shape.setPosition(x - size.x / 2, y - size.y / 2);
}

int TrackingBox::update(canvas_t& canvas) {
	auto mouse = sf::Mouse::getPosition(canvas);
	mouse = (sf::Vector2i)canvas.mapPixelToCoords(mouse);
	auto size = _shape.getSize();

	_shape.setPosition(
		mouse.x - size.x / 2 >= 0 && mouse.x + size.x / 2 <= canvas.getSize().x
		? mouse.x - size.x / 2
		: _shape.getPosition().x,

		mouse.y - size.y / 2 >= 0 && mouse.y + size.y / 2 <= canvas.getSize().y
		? mouse.y - size.y / 2
		: _shape.getPosition().y
	);

	return (int)UpdateCodes::IDLE;
}

void TrackingBox::draw_to(target_t& canvas) {
	canvas.draw(_shape);
}

void TrackingBox::draw_to(canvas_t& canvas) {
	if (MouseInView(canvas))
		canvas.draw(_shape);
}

model_t TrackingBox::get_boundaries(const Geometry2D& geo) const {
	auto rect = _shape.getGlobalBounds();
	return model_t{
		geo.coord_x((int_t)rect.left),
		geo.coord_x((int_t)rect.left + (int_t)rect.width),
		geo.coord_y((int_t)rect.top),
		geo.coord_y((int_t)rect.top + (int_t)rect.height)
	};
}


InputBox::InputBox(font_t& font):
	MenuBox(font)
{
	add_text_item(
		font,
		INPUT_BOX_FONT_POINT,
		MenuBox::DEFAULT_FILL_COLOR,
		INIT_X_POS + INPUT_BOX_X,
		INIT_Y_POS + INPUT_BOX_Y
	);
}

int InputBox::update(canvas_t& window) {
	return (int)UpdateCodes::IDLE;
}

std::string InputBox::get() const {
	return _labels.back().getString();
}

void InputBox::set(const std::string& msg) {
	_labels.back().setString(msg);
}
