﻿#include "WorldTransform.h"

void WorldTransform::Initialize()
{
	matWorld = MatrixTransform::Identity();
	
}

void WorldTransform::UpdateMatrix()
{
	matWorld = MatrixTransform::AffineMatrix(scale, rotation, translate);
}

void WorldTransform::TransfarMatrix(ComPtr<ID3D12Resource>& wvpResource, ViewProjection viewProjection, Projection Flag )
{
	TransformationMatrix* wvp = {};
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvp));
	Matrix4x4 wvpMatrix = MatrixTransform::MakeIdenttity4x4();
	if (Flag == PerspectiveFov)
	{
		wvpMatrix = MatrixTransform::Multiply(matWorld, MatrixTransform::Multiply(viewProjection.matView_, viewProjection.matProjection_));
	}
	if (Flag==OrthographicMatrix)
	{
		Matrix4x4 viewMatrix = MatrixTransform::Identity();
		Matrix4x4 OrthographicMatrix = MatrixTransform::OrthographicMatrix(
			0, 0, float(WinApp::GetkCilientWidth()), float(WinApp::GetkCilientHeight()), 0.0f, 100.0f);

		wvpMatrix =MatrixTransform::Multiply(matWorld,MatrixTransform::Multiply(viewMatrix,OrthographicMatrix));
	}
	wvp->WVP = wvpMatrix;
	wvp->world = matWorld;
	
}
