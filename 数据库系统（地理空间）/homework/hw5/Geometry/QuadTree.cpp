#include <set>
#include "QuadTree.h"
#include <map>

namespace lab3 {

/*
 * QuadNode
 */
void QuadNode::split(size_t capacity)
{
	for (int i = 0; i < 4; ++i) {
		delete []nodes[i];
		nodes[i] = NULL;
	}
	
	if (features.size() > capacity) {
		double midx = (bbox.getMinX() + bbox.getMaxX()) / 2.0;
		double midy = (bbox.getMinY() + bbox.getMaxY()) / 2.0;
		Envelope e0(bbox.getMinX(), midx, midy, bbox.getMaxY());
		nodes[0] = new QuadNode(e0);
		Envelope e1(midx, bbox.getMaxX(), midy, bbox.getMaxY());
		nodes[1] = new QuadNode(e1);
		Envelope e2(midx, bbox.getMaxX(), bbox.getMinY(), midy);
		nodes[2] = new QuadNode(e2);
		Envelope e3(bbox.getMinX(), midx, bbox.getMinY(), midy);
		nodes[3] = new QuadNode(e3);

		for (Feature f : features) {
			for (int i = 0; i < 4; ++i) {
				if (nodes[i]->getEnvelope().intersect(f.getEnvelope())) {
					nodes[i]->add(f);
				}
			}
		}

		for (int i = 0; i < 4; ++i) {
			nodes[i]->split(capacity);
		}
		features.clear();
	}
}

void QuadNode::countNode(int& interiorNum, int& leafNum)
{
	if (isLeafNode()) {
		++leafNum;
	}
	else {
		++interiorNum;
		for (int i = 0; i < 4; ++i)
			nodes[i]->countNode(interiorNum, leafNum);
	}
}

int QuadNode::countHeight(int height)
{
	++height;
	if (!isLeafNode()) {
		int cur = height;
		for (int i = 0; i < 4; ++i) {
			height = max(height, nodes[i]->countHeight(cur));
		}
	}
	return height;
}

void QuadNode::rangeQuery(Envelope& rect, vector<Feature>& features)
{
	if (!bbox.intersect(rect))
		return;
	// Task 6.2 range query
	// Write your code here
	if (isLeafNode()) {
		for (auto f : this->features) {
			if (f.getEnvelope().intersect(rect)) {
					features.push_back(f);
			}
		}
	}
	else {
		for (int i = 0; i < 4; ++i) {
			nodes[i]->rangeQuery(rect, features);
		}
	}
}

QuadNode* QuadNode::pointInLeafNode(double x, double y)
{
	// Task 7.2 NN query
	// Write your code here
	if (isLeafNode()) {
		return this;
	}
	else {
		for (int i = 0; i < 4; ++i) {
			if (nodes[i]->bbox.contain(x, y)) {
				return nodes[i]->pointInLeafNode(x, y);
			}
		}
	}
	return NULL;
}
/*
void QuadNode::draw()
{
	if (isLeafNode()) {
		bbox.draw();
	}
	else {
		for (int i = 0; i < 4; ++i)
			nodes[i]->draw();
	}
}
*/
/*
 * QuadTree
 */
bool QuadTree::constructQuadTree(vector<Feature>& features)
{
	if (features.empty())
		return false;

	// Task 5.1 construction
	// Write your code here

	Envelope e(DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX);
	for (Feature f : features) {
		e = e.unionEnvelope(f.getEnvelope());
	}
	root = new QuadNode(e);

	root->add(features);

	root->split(capacity);

	bbox = e; // ע����д�����Ҫ����Ϊfeatures�İ�Χ�У�����ڵ�İ�Χ��
	//bbox = Envelope(-74.1, -73.8, 40.6, 40.8);
	return true;
}

void QuadTree::countQuadNode(int& interiorNum, int& leafNum)
{
	interiorNum = 0;
	leafNum = 0;
	if (root)
		root->countNode(interiorNum, leafNum);
}

void QuadTree::countHeight(int &height)
{
	height = 0;
	if (root)
		height = root->countHeight(0);
}

void QuadTree::rangeQuery(Envelope& rect, vector<Feature>& features) 
{ 
	features.clear();

	// Task 6.1 range query
	// Write your code here
	// filter step (ѡ���ѯ�����뼸�ζ����Χ���ཻ�ļ��ζ���)
	vector<Feature> features1;
	root->rangeQuery(rect, features1);
	// refine step (��ȷ�ж�ʱ����Ҫȥ�أ������ѯ����ͼ��ζ�����ظ�����)

	set<Feature> featureMap;
	for (auto f : features1) {
		if (featureMap.find(f) == featureMap.end()) {
			if(f.getGeom()->intersects(rect)) // refine ��ȷ�ж�
				features.push_back(f);
			featureMap.insert(f);
		}
	}
}

bool QuadTree::NNQuery(double x, double y, Feature& feature)
{
	if (!root || !(root->getEnvelope().contain(x, y)))
		return false;

	// Task 7.1 NN query
	// Write your code here

	// filter step (ʹ��maxDistance2Envelope��������ò�ѯ�㵽���ζ����Χ�е���̵������룬Ȼ�������ѯ��ú�ѡ��)

	const Envelope& envelope = root->getEnvelope();
	double minDist = max(envelope.getWidth(), envelope.getHeight());

	QuadNode* n = root->pointInLeafNode(x, y);

	for (int i = 0; i < n->getFeatureNum(); ++i) {
		minDist = min(minDist,n->getFeature(i).maxDistance2Envelope(x,y));
	}
	Envelope rect = Envelope(x - minDist, x + minDist, y - minDist, y + minDist);
	vector< Feature> features;
	rangeQuery(rect, features);
	// refine step (��ȷ�����ѯ���뼸�ζ���ľ���)
	double dist;
	for (Feature f : features) {
		dist = f.getGeom()->distance(&Point(x, y));
		if (dist <= minDist) {
			feature = f;
			minDist = dist;
		}
	}

	return true;
}
/*
void QuadTree::draw()
{
	if (root)
		root->draw();
}
*/
}
