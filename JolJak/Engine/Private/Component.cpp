#include "Component.h"

CComponent::CComponent()
{
}

CComponent::CComponent(const CComponent& rhs)
{
}

void CComponent::Free()
{
}

CComponent_DC::CComponent_DC(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) : m_Device(pDevice), m_CommandList(pCommandList)
{
	Safe_AddRef(m_Device);
	Safe_AddRef(m_CommandList);
}

CComponent_DC::CComponent_DC(const CComponent_DC& rhs) : m_Device(rhs.m_Device), m_CommandList(rhs.m_CommandList)
{
	Safe_AddRef(m_Device);
	Safe_AddRef(m_CommandList);
}

void CComponent_DC::Free()
{
	Safe_Release(m_Device);
	Safe_Release(m_CommandList);
	__super::Free();
}