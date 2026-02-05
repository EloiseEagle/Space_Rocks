#pragma once
#include "HtGraphics.h"
#include "HtCamera.h"

HtCamera HtCamera::instance;

HtCamera::HtCamera()
{
	Reset();
}

void HtCamera::Reset()
{
	int height = HtGraphics::instance.GetWindowHeight();
	int width = HtGraphics::instance.GetWindowWidth();

	m_screenCentre.set(width/2.0, height/2.0);
	m_screenZoom = height/2000.0;
	m_worldZoom = 1.0;
	m_worldPosition.set(0, 0);
	m_active = true;
	CalculateArea();
}

void HtCamera::PlaceAt(Vector2D worldPosition)
{
	m_worldPosition = worldPosition;
	CalculateArea();
}

Vector2D HtCamera::GetCameraCentre(Vector2D worldPosition)
{
	return m_worldPosition;
}

void HtCamera::SetZoom(double zoom)
{
	m_worldZoom = zoom;
	CalculateArea();
}

double HtCamera::GetZoom() const
{
	return m_worldZoom;
}

Rectangle2D HtCamera::GetCameraArea() const
{
	return 	m_cameraArea;
}

double HtCamera::GetTopOfCameraArea() const
{
	return m_cameraArea.GetTopRight().YValue;
}

double HtCamera::GetBottomOfCameraArea() const
{
	return m_cameraArea.GetBottomLeft().YValue;
}

double HtCamera::GetLeftOfCameraArea() const
{
	return m_cameraArea.GetBottomLeft().XValue;
}

double HtCamera::GetRightOfCameraArea() const
{
	return m_cameraArea.GetTopRight().XValue;
}

double HtCamera::NativeTransform(double gameSize)
{
	if (m_active)
		return GetScaleFromWorld(gameSize);
	else
		return GetScaleFromScreen(gameSize);
}

Vector2D HtCamera::NativeTransform(Vector2D gamePosition)
{
	if (m_active)
		return GetNativeFromWorldCoordinates(gamePosition);
	else
		return GetNativeFromScreenCoordinates(gamePosition);
}

Circle2D HtCamera::NativeTransform(Circle2D gameCircle)
{
	if (m_active)
		return GetCircleFromWorld(gameCircle);
	else
		return GetCircleFromScreen(gameCircle);
}

Rectangle2D HtCamera::NativeTransform(Rectangle2D gameRectangle)
{
	if (m_active)
		return GetRectangleFromWorld(gameRectangle);
	else
		return GetRectangleFromScreen(gameRectangle);
}

AngledRectangle2D HtCamera::NativeTransform(AngledRectangle2D gameRectangle)
{
	if (m_active)
		return GetAngledRectangleFromWorld(gameRectangle);
	else
		return GetAngledRectangleFromScreen(gameRectangle);
}

Segment2D HtCamera::NativeTransform(Segment2D gameSegment)
{
	if (m_active)
		return GetSegmentFromWorld(gameSegment);
	else
		return GetSegmentFromScreen(gameSegment);
}

double HtCamera::GameTransform(double nativeSize)
{
	if (m_active)
		return GetWorldSize(nativeSize);
	else
		return GetScreenSize(nativeSize);
}

Vector2D HtCamera::GameTransform(Vector2D nativePosition)
{
	if (m_active)
		return GetWorldCoordinates(nativePosition);
	else
		return GetScreenCoordinates(nativePosition);
}

void HtCamera::UseCamera(bool cameraUsed)
{
	m_active = cameraUsed;
}

bool HtCamera::IsOnCamera(Vector2D point) const
{
	return m_cameraArea.Intersects(point);
}

bool HtCamera::IsCameraActive() const
{
	return m_active;
}

Vector2D HtCamera::GetNativeFromWorldCoordinates(Vector2D worldPosition) const
{
	worldPosition = worldPosition - m_worldPosition;
	worldPosition.YValue = -worldPosition.YValue;
	worldPosition = m_screenZoom * m_worldZoom * worldPosition + m_screenCentre;

	return worldPosition;
}

Vector2D HtCamera::GetNativeFromScreenCoordinates(Vector2D screenPosition) const
{
	screenPosition.YValue = -screenPosition.YValue;
	screenPosition = m_screenZoom * screenPosition  + m_screenCentre;
	return screenPosition;
}

Vector2D HtCamera::GetWorldCoordinates(Vector2D nativeCoordinates) const
{
	nativeCoordinates = (nativeCoordinates - m_screenCentre) / (m_screenZoom * m_worldZoom) ;
	nativeCoordinates.YValue = -nativeCoordinates.YValue;
	nativeCoordinates = m_worldPosition + nativeCoordinates;

	return nativeCoordinates;
}

Vector2D HtCamera::GetScreenCoordinates(Vector2D nativeCoordinates) const
{
	nativeCoordinates = (nativeCoordinates-m_screenCentre)  / m_screenZoom ;
	nativeCoordinates.YValue = -nativeCoordinates.YValue;
	return nativeCoordinates;
}

double HtCamera::GetScaleFromWorld(double size) const
{
	return size * m_screenZoom * m_worldZoom;
}

double HtCamera::GetScaleFromScreen(double size) const
{
	return size * m_screenZoom;
}

double HtCamera::GetWorldSize(double nativesize) const
{
	return nativesize / (m_screenZoom * m_worldZoom);
}

double HtCamera::GetScreenSize(double nativesize) const
{
	return nativesize / m_screenZoom;
}

Circle2D HtCamera::GetCircleFromWorld(const Circle2D& circle) const
{
	return Circle2D(GetNativeFromWorldCoordinates(circle.GetCentre()), GetScaleFromWorld(circle.GetRadius()));
}

Circle2D HtCamera::GetCircleFromScreen(const Circle2D& circle) const
{
	return Circle2D(GetNativeFromScreenCoordinates(circle.GetCentre()), GetScaleFromScreen(circle.GetRadius()));
}

Rectangle2D HtCamera::GetRectangleFromWorld(const Rectangle2D& rectangle) const
{
	Rectangle2D answer;
	answer.PlaceAt(GetNativeFromWorldCoordinates(rectangle.GetBottomLeft()), 
		GetNativeFromWorldCoordinates(rectangle.GetTopRight()));
	return answer;
}

Rectangle2D HtCamera::GetRectangleFromScreen(const Rectangle2D& rectangle) const
{
	Rectangle2D answer;
	answer.PlaceAt(GetNativeFromScreenCoordinates(rectangle.GetBottomLeft()),
		GetNativeFromScreenCoordinates(rectangle.GetTopRight()));
	return answer;
}

AngledRectangle2D HtCamera::GetAngledRectangleFromWorld(const AngledRectangle2D& rectangle) const
{
	AngledRectangle2D answer;
	answer.SetCentre(GetNativeFromWorldCoordinates(rectangle.GetCentre()));
	answer.SetDimensions(GetScaleFromWorld(rectangle.GetHeight()), 
						GetScaleFromWorld(rectangle.GetWidth()));
	answer.SetAngle(-rectangle.GetAngle());

	return answer;
}

AngledRectangle2D HtCamera::GetAngledRectangleFromScreen(const AngledRectangle2D& rectangle) const
{
	AngledRectangle2D answer;
	answer.SetCentre(GetNativeFromScreenCoordinates(rectangle.GetCentre()));
	answer.SetDimensions(GetScaleFromScreen(rectangle.GetHeight()),
		GetScaleFromWorld(rectangle.GetWidth()));
	answer.SetAngle(-rectangle.GetAngle());

	return answer;
}

Segment2D HtCamera::GetSegmentFromWorld(const Segment2D& segment) const
{
	Segment2D answer;
	answer.PlaceAt(GetNativeFromWorldCoordinates(segment.GetStart()), 
		GetNativeFromWorldCoordinates(segment.GetEnd()));
	return answer;
}

Segment2D HtCamera::GetSegmentFromScreen(const Segment2D& segment) const
{
	Segment2D answer;
	answer.PlaceAt(GetNativeFromScreenCoordinates(segment.GetStart()),
		GetNativeFromScreenCoordinates(segment.GetEnd()));
	return answer;
}

void HtCamera::CalculateArea()
{
	Vector2D bottomLeft(0, HtGraphics::instance.GetWindowHeight());
	Vector2D topRight(HtGraphics::instance.GetWindowWidth(), 0);
	m_cameraArea = Rectangle2D(GetWorldCoordinates(bottomLeft), GetWorldCoordinates(topRight));
}
