/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "renderer/CCCustomCommand.h"
#include "renderer/CCTextureAtlas.h"
#include "renderer/backend/Buffer.h"
#include "renderer/backend/Device.h"
#include "base/ccUtils.h"

NS_CC_BEGIN

CustomCommand::CustomCommand()
{
    _type = RenderCommand::Type::CUSTOM_COMMAND;
}

CustomCommand::CustomCommand(const CustomCommand &other) : RenderCommand(other)
{
    copyInternal(other);
}

CustomCommand& CustomCommand::operator=(const CustomCommand &other)
{
    RenderCommand::operator=(other);
    
    cleanupInternal();
    copyInternal(other);
    
    return *this;
}

CustomCommand::~CustomCommand()
{
    cleanupInternal();
}

void CustomCommand::copyInternal(const CustomCommand &other)
{
    _vertexBuffer = other._vertexBuffer;
    _indexBuffer = other._indexBuffer;
    
    _vertexDrawStart = other._vertexDrawStart;
    _vertexDrawCount = other._vertexDrawCount;
    
    _indexDrawOffset = other._indexDrawOffset;
    _indexDrawCount = other._indexDrawCount;
    
    _drawType = other._drawType;
    _primitiveType = other._primitiveType;
    _indexFormat = other._indexFormat;
    
    _lineWidth = other._lineWidth;
    
    _indexSize = other._indexSize;
    
    _vertexCapacity = other._vertexCapacity;
    _indexCapacity = other._indexCapacity;
    
    _beforeCallback = other._beforeCallback;
    _afterCallback = other._afterCallback;
    
    CC_SAFE_RETAIN(_vertexBuffer);
    CC_SAFE_RETAIN(_indexBuffer);
}

void CustomCommand::cleanupInternal()
{
    CC_SAFE_RELEASE_NULL(_vertexBuffer);
    CC_SAFE_RELEASE_NULL(_indexBuffer);
}

void CustomCommand::init(float depth, const cocos2d::Mat4 &modelViewTransform, unsigned int flags)
{
    RenderCommand::init(depth, modelViewTransform, flags);
}

void CustomCommand::init(float globalZOrder)
{
    _globalOrder = globalZOrder;
}

void CustomCommand::init(float globalZOrder, const BlendFunc& blendFunc)
{
    _globalOrder = globalZOrder;

    auto& blendDescriptor = _pipelineDescriptor.blendDescriptor;
    blendDescriptor.blendEnabled = true;
    blendDescriptor.sourceRGBBlendFactor = blendDescriptor.sourceAlphaBlendFactor = blendFunc.src;
    blendDescriptor.destinationRGBBlendFactor = blendDescriptor.destinationAlphaBlendFactor = blendFunc.dst;
}

void CustomCommand::createVertexBuffer(unsigned int vertexSize, unsigned int capacity, BufferUsage usage)
{
    CC_SAFE_RELEASE(_vertexBuffer);
    
    _vertexCapacity = capacity;
    _vertexDrawCount = capacity;
    
    auto device = backend::Device::getInstance();
    _vertexBuffer = device->newBuffer(vertexSize * capacity, backend::BufferType::VERTEX, usage);
}

void CustomCommand::createIndexBuffer(IndexFormat format, unsigned int capacity, BufferUsage usage)
{
    CC_SAFE_RELEASE(_indexBuffer);
    
    _indexFormat = format;
    _indexSize = computeIndexSize();
    _indexCapacity = capacity;
    _indexDrawCount = capacity;
    
    auto device = backend::Device::getInstance();
    _indexBuffer = device->newBuffer(_indexSize * capacity, backend::BufferType::INDEX, usage);
}

void CustomCommand::updateVertexBuffer(void* data, unsigned int offset, unsigned int length)
{   
    assert(_vertexBuffer);
    _vertexBuffer->updateSubData(data, offset, length);
}

void CustomCommand::updateIndexBuffer(void* data, unsigned int offset, unsigned int length)
{
    assert(_indexBuffer);
    _indexBuffer->updateSubData(data, offset, length);
}

void CustomCommand::setVertexBuffer(backend::Buffer *vertexBuffer)
{
    if (_vertexBuffer == vertexBuffer)
        return;

    _vertexBuffer = vertexBuffer;
    CC_SAFE_RETAIN(_vertexBuffer);
}

void CustomCommand::setIndexBuffer(backend::Buffer *indexBuffer, IndexFormat format)
{
    if (_indexBuffer == indexBuffer && _indexFormat == format)
        return;

    _indexBuffer = indexBuffer;
    CC_SAFE_RETAIN(_indexBuffer);

    _indexFormat = format;
    _indexSize = computeIndexSize();
}

void CustomCommand::updateVertexBuffer(void* data, unsigned int length)
{
    assert(_vertexBuffer);
    _vertexBuffer->updateData(data, length);
}

void CustomCommand::updateIndexBuffer(void* data, unsigned int length)
{
    assert(_indexBuffer);
    _indexBuffer->updateData(data, length);
}

unsigned int CustomCommand::computeIndexSize() const
{
if (IndexFormat::U_SHORT == _indexFormat)
    return (unsigned int)sizeof(unsigned short);
else
    return (unsigned int)sizeof(unsigned int);
}

NS_CC_END
