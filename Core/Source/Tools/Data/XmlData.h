#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\List.h>

class XmlData
{
public:
	struct AttributeName
	{
		AttributeName(const char* name) : name(name) {}

		inline bool operator==(const AttributeName& other) const { return name == other.name; }
		inline bool operator!=(const AttributeName& other) const { return name != other.name; }
		inline operator const char*() const { return name; }

		const char* name;
	};

	struct AttributeValue
	{
		AttributeValue(int32_t data) : data((void*) data) {}
		AttributeValue(void* data) : data((void*) data) {}
		AttributeValue(const char* data) : data((void*) data) {}

		inline operator int32_t() const { return (int32_t) data; }
		inline operator void*() const { return data; }
		inline operator const char*() const { return (const char*) data; }

		inline int32_t GetInt32() const { return (int32_t) data; }
		inline void* GetPtr() const { return data; }
		inline const char* GetString() const { return (const char*) data; }

		void* data;
	};

	struct Attribute
	{
		Attribute(const AttributeName& name, const AttributeValue& value) : name(name), value(value) {}

		AttributeName name;
		AttributeValue value;
	};

	class AttributeMap
	{
	public:
		Attribute& Add(const AttributeName& name, const AttributeValue& value) 
		{
			items.push_back(Attribute(name, value)); 
			return items.back();
		}
		Attribute& operator[](int index) { return items.at(index); }
		size_t Count() const { return items.size(); }
		void Remove(List<Attribute>::iterator& where) { items.erase(where); }

		List<Attribute>::iterator Find(const AttributeName& name, const List<Attribute>::iterator& from, const List<Attribute>::iterator& to)
		{
			return std::find(from, to, [name](Attribute& item)
			{
				return item.name == name;
			});
		}
		List<Attribute>::iterator Find(const AttributeName& name)
		{
			return std::find(Begin(), End(), [name](Attribute& item)
			{
				return item.name == name;
			});
		}
		List<Attribute>::iterator Begin() { return items.begin(); }
		List<Attribute>::iterator End() { return items.end(); }

	private:
		List<Attribute> items;
	};

	struct ElementName
	{
		ElementName(const char* name) : name(name) {}

		inline bool operator==(const ElementName& other) const { return name == other.name; }
		inline bool operator!=(const ElementName& other) const { return name != other.name; }
		inline operator const char*() const { return name; }

		const char* name;
	};

	class Element;
	class ElementMap
	{
	public:
		ElementMap(Element* element) : element(element) {}
		Element* AddOrFind(const ElementName& name)
		{
			auto current = Find(name, Begin(), End());
			return current == End() ? Add(name) : *current;
		}
		Element* Add(const ElementName& name) 
		{ 
			items.push_back(new Element(name, element));
			return items.back();
		}
		Element* operator[](int index) { return items.at(index); }
		size_t Count() const { return items.size(); }
		void Remove(List<Element*>::iterator& where) { items.erase(where); }

		List<Element*>::iterator Find(const ElementName& name, const List<Element*>::iterator& from, const List<Element*>::iterator& to)
		{
			return std::find(from, to, [name](Element* item)
			{
				return item->GetName() == name;
			});
		}
		List<Element*>::iterator Find(const ElementName& name)
		{
			return std::find(Begin(), End(), [name](Element* item)
			{
				return item->GetName() == name;
			});
		}
		List<Element*>::iterator Begin() { return items.begin(); }
		List<Element*>::iterator End() { return items.end(); }

	private:
		Element* element;
		List<Element*> items;
	};

	class Element
	{
	public:
		Element(const ElementName& name, Element* parent) : name(name), parent(parent), elements(this) {}
		bool HasElements() const { return elements.Count() != 0; }
		ElementMap& GetElements() { return elements; }
		bool HasAttributes() const { return attributes.Count() != 0; }
		AttributeMap& GetAttributes() { return attributes; }
		bool HasParent() { return parent != nullptr; }
		Element* GetParent() const { ASSERT(parent != nullptr); return parent; }
		const ElementName& GetName() const { return name; }

	private:
		ElementName name;
		Element* parent;
		ElementMap elements;
		AttributeMap attributes;
	};

public:
	XmlData() : element(ElementName("Root"), nullptr) {}
	Element* GetElement() { return &element; }

private:
	Element element;
};