#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetWindowTitle("openframeworks");

	ofBackground(239);

	this->radius = 15;
	vector<glm::vec2> start_location_list;
	auto x_span = this->radius * sqrt(3);
	auto flg = true;
	auto edge = 120;
	for (float y = edge; y < ofGetHeight() + this->radius - edge; y += this->radius * 1.5) {

		for (float x = edge; x < ofGetWidth() + this->radius - edge; x += x_span) {

			glm::vec2 location;
			if (flg) {

				location = glm::vec2(x, y);
			}
			else {

				location = glm::vec2(x + (this->radius * sqrt(3) / 2), y);
			}

			start_location_list.push_back(location);
		}
		flg = !flg;
	}

	for (int i = 0; i < 10; i++) {

		int r = ofRandom(start_location_list.size());

		auto log_list = vector<glm::vec2>();
		log_list.push_back(start_location_list[r]);
		this->hexagon_list.push_back(log_list);
	}

	ofColor color;
	vector<int> hex_list = { 0xee6352, 0x59cd90, 0x3fa7d6, 0xfac05e, 0xf79d84 };
	for (auto hex : hex_list) {

		color.setHex(hex);
		this->base_color_list.push_back(color);
	}

	this->fbo.allocate(ofGetWidth() + 25, ofGetHeight() + 25);
	this->fbo.readToPixels(this->pixels);
	this->pixels_mat = cv::Mat(this->pixels.getHeight(), this->pixels.getWidth(), CV_8UC4, this->pixels.getData());
}

//--------------------------------------------------------------
void ofApp::update() {

	auto span = this->radius * sqrt(3);
	auto edge = 120;
	for (auto& log_list : this->hexagon_list) {

		int r = ofRandom(6);
		int deg_start = r * 60;
		for (int deg = deg_start; deg < deg_start + 360; deg += 60) {

			auto tmp_location = log_list.back() + glm::vec2(span * cos(deg * DEG_TO_RAD), span * sin(deg * DEG_TO_RAD));
			if (tmp_location.x < edge || tmp_location.x > ofGetWidth() + this->radius - edge || tmp_location.y < edge || tmp_location.y > ofGetHeight() + this->radius - edge) {

				continue;
			}

			log_list.push_back(tmp_location);
			break;
		}

		while (log_list.size() > 10) {

			log_list.erase(log_list.begin());
		}
	}

	this->fbo.begin();
	ofClear(0);
	ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ADD);

	int color_index = 0;
	for (auto& log_list : this->hexagon_list) {

		for (int i = 0; i < log_list.size(); i++) {

			vector<glm::vec2> vertices;
			for (auto deg = 90; deg < 450; deg += 60) {

				vertices.push_back(log_list[i] + glm::vec2(this->radius * cos(deg * DEG_TO_RAD), this->radius * sin(deg * DEG_TO_RAD)));
			}

			ofFill();
			ofSetColor(this->base_color_list[color_index % this->base_color_list.size()]);
			ofBeginShape();
			ofVertices(vertices);
			ofEndShape(true);

			ofNoFill();
			ofSetColor(255);
			ofBeginShape();
			ofVertices(vertices);
			ofEndShape(true);
		}

		color_index++;
	}

	ofDisableBlendMode();

	this->fbo.end();
	this->fbo.readToPixels(this->pixels);

	cv::Mat gray_mat, bit_mat;
	cv::cvtColor(this->pixels_mat, gray_mat, cv::COLOR_RGB2GRAY);
	for (int y = 0; y < this->pixels_mat.cols; y++) {

		for (int x = 0; x < this->pixels_mat.rows; x++) {

			if (gray_mat.at<unsigned char>(y, x) > 0) {

				this->pixels_mat.at<cv::Vec4b>(y, x) = cv::Vec4b(139, 139, 139, 255);
			}
		}
	}

	cv::GaussianBlur(this->pixels_mat, this->pixels_mat, cv::Size(19, 19), 10, 10);
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofSetColor(255);

	ofImage draw_image;
	draw_image.setFromPixels(this->pixels);
	draw_image.draw(0, 0);

	this->fbo.draw(-15, -15);
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}