/*!
 *  Auto-generated types for schema definition [source schema: test_schema]
 *
 *  Reference Schema Version : 0001.0000
 *  Generated by : subho
 *  Generated on : 2017-01-30 14:58:18.665
 */


#ifndef TEST_REF_TYPE_H
#define TEST_REF_TYPE_H
#include "types/includes/type_defs.h"
#include "types/includes/__base_type.h"

#include "test_type.h"


using namespace REACTFS_NS_COMMON_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX::types;



namespace com {
	namespace wookler {
		namespace test {
			/**
			* Generated code for data record type test_ref_type.
			*
			* Note:: Should not be modified as the changes will be lost when the code is re-generated.
			*/
			class test_ref_type  : public com::wookler::reactfs::core::types::__base_type
			{
			private:
				const char * name = nullptr;

				std::unordered_map<std::string, test_type *> * testRefMap = nullptr;



				/**
				* Value name setter from the the serialized data map.
				*
				* @param __data - Serialized data map pointer.
				*/
				void read_value_name ( const record_struct *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						const void *__ptr = __data->get_field ( 0 );
						if ( NOT_NULL ( __ptr ) ) {
							// Set the value of name
							const char * __var = static_cast<const char *> ( __ptr );
							CHECK_CAST ( __var, TYPE_NAME ( void * ), TYPE_NAME ( char ) );
							this->name = __var;

						} else {
							this->name = nullptr;
						}
					} else {
						this->name = nullptr;
					}
				}

				/**
				 * Deserialize an instance of type test_type from the
				 * passed input data.
				 *
				 * @param __input - Void pointer to input data of type record_struct
				 * @return - Deserialized instance pointer of type test_type *
				 */
				test_type * deserialize_test_type ( const void *__input )
				{
					CHECK_NOT_NULL ( __input );
					const record_struct *__value = static_cast<const record_struct *> ( __input );
					CHECK_CAST ( __value, TYPE_NAME ( void * ), TYPE_NAME ( record_struct ) );
					test_type * __data = new test_type();
					__data->deserialize ( __value );
					CHECK_ALLOC ( __data, TYPE_NAME ( test_type ) );
					return __data;
				}

				/**
				 * Deserialize a map instance of key/value type std::string/test_type * from the passed input data.
				 *
				 * @param __input - Void pointer of type unordered_map<std::string, record_struct *>
				 * @return - Deserialized map of key/value type unordered_map<std::string, test_type *>
				 */
				std::unordered_map<std::string, test_type *> *deserialize_map_std__unordered_map_std__string__test_type___ ( const void *__input )
				{
					CHECK_NOT_NULL ( __input );
					const std::unordered_map<std::string, record_struct *> *__value = static_cast<const std::unordered_map<std::string, record_struct *> *> ( __input );
					CHECK_CAST ( __value, TYPE_NAME ( void * ), TYPE_NAME ( unordered_map ) );

					std::unordered_map<std::string, test_type *> *__map = new std::unordered_map<std::string, test_type *>();
					CHECK_ALLOC ( __map, TYPE_NAME ( unordered_map ) );

					std::unordered_map<std::string, record_struct *>::const_iterator iter;
					for ( iter = __value->begin(); iter != __value->end(); iter++ ) {
						test_type * __tv = deserialize_test_type ( iter->second );
						CHECK_NOT_NULL ( __tv );
						__map->insert ( {iter->first, __tv} );
					}
					return __map;
				}

				/**
				 * Value testRefMap setter from the the serialized data map.
				 *
				 * @param __data - Serialized data map pointer.
				 */
				void read_value_testRefMap ( const record_struct *__data )
				{
					if ( NOT_NULL ( __data ) ) {
						const void *__ptr = __data->get_field ( 1 );
						if ( NOT_NULL ( __ptr ) ) {
							// Set the value of map testRefMap
							std::unordered_map<std::string, test_type *> *__var = deserialize_map_std__unordered_map_std__string__test_type___ ( __ptr );
							CHECK_NOT_NULL ( __var );
							this->testRefMap = __var;

						} else {
							this->testRefMap = nullptr;
						}
					} else {
						this->testRefMap = nullptr;
					}
				}


			public:
				/**
				* Get the pointer to the property name of type char *.
				* Returns a const pointer.
				*
				* @return char *
				*/
				const char * get_name ( void ) const
				{
					return this->name;
				}

				/**
				 * Get the pointer to the property testRefMap of type std::unordered_map<std::string, test_type *> *.
				 * Returns a const pointer.
				 *
				 * @return std::unordered_map<std::string, test_type *> *
				 */
				const std::unordered_map<std::string, test_type *> * get_testRefMap ( void ) const
				{
					return this->testRefMap;
				}

				/**
				 * Create a new read-only instance of test_ref_type.
				 *
				 * Type to be used when instance is being de-serialized from record data.
				 */
				test_ref_type()
				{
					this->name = nullptr;
					this->testRefMap = nullptr;

				}

				/**
				 * Descructor for test_ref_type
				 */
				~test_ref_type()
				{
					FREE_TYPE_MAP ( this->testRefMap );


				}

				/**
				 * Deserialize this type instance from the passed data map.
				 *
				 * @param __data - Serialized data record instance to load this type from.
				 */
				void deserialize ( const record_struct *__data ) override
				{
					CHECK_NOT_NULL ( __data );

					this->record_type = __data->get_record_type();
					CHECK_NOT_NULL ( this->record_type );

					// Set the value of name from the serialized data map.
					this->read_value_name ( __data );

					// Set the value of testRefMap from the serialized data map.
					this->read_value_testRefMap ( __data );



					this->__data = __data;
				}

				/**
				 * Compare this type instance to the passed instance.
				 *
				 * @param __target - Target instance to compare with. If pointer is not
				 * 			of the same type will throw exception.
				 * @return - Is equal?
				 */
				bool equals ( const void* __target ) const override
				{
					if ( IS_NULL ( __target ) ) {
						return false;
					}
					const test_ref_type * __t = static_cast<const test_ref_type *> ( __target );
					CHECK_CAST ( __t, TYPE_NAME ( void * ), TYPE_NAME ( test_ref_type * ) );

					{
						// Compare field name
						const __native_type *nt = get_field_type ( "name" );
						CHECK_NOT_NULL ( nt );
						__base_datatype_io *th = nt->get_type_handler ( __record_mode::RM_READ );
						CHECK_NOT_NULL ( th );
						const void *tv = __t->get_name();
						const void *sv = this->get_name();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
							return true;
						}
						bool r = th->compare ( tv, sv, __constraint_operator::EQ );
						if ( !r ) {
							return false;
						}
					}

					{
						// Compare map instance testRefMap
						const unordered_map<std::string, test_type *> *tv = __t->get_testRefMap();
						const unordered_map<std::string, test_type *> *sv = this->get_testRefMap();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
							return true;
						}
						if ( tv->size() != sv->size() ) {
							return false;
						}

						unordered_map<std::string, test_type *>::const_iterator iter_t;
						unordered_map<std::string, test_type *>::const_iterator iter_s;

						for ( iter_s = sv->begin(); iter_s != sv->end(); iter_s++ ) {
							iter_t = tv->find ( iter_s->first );
							if ( iter_t == tv->end() ) {
								return false;
							}
							const test_type * v1 = iter_s->second;
							const test_type * v2 = iter_t->second;
							if ( IS_NULL ( v1 ) ) {
								if ( NOT_NULL ( v2 ) ) {
									return false;
								}
								continue;
							}
							// Check value type equals.
							bool r = v1->equals ( v2 );
							if ( !r ) {
								return false;
							}
						}
					}



					return true;
				}


			};


			/**
			 * Generated code for data record type mutable_test_ref_type.
			 *
			 * Note:: Should not be modified as the changes will be lost when the code is re-generated.
			 */
			class mutable_test_ref_type  : public com::wookler::reactfs::core::types::__mutable_base_type
			{
			private:
				char * name = nullptr;

				std::unordered_map<std::string, mutable_test_type *> * testRefMap = nullptr;



				/**
				* Method extracts and set the value of name from
				* the serialized data map.
				*
				* @param __data - Serialized data map pointer.
				*/
				void set_serde_name ( mutable_record_struct *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->name ) ) {
						// Add the value of name to the data map.
						{
							const __native_type *ft = get_field_type ( "name" );
							CHECK_NOT_NULL ( ft );
							__data->add_field ( ft->get_index(), this->name );

						}

					}
				}

				/**
				 * Serialize the type value for mutable_test_type and add to the data map.
				 *
				 * @param __value - Pointer to the field of type mutable_test_type.
				 * @return - Serialized data map instance.
				 */
				mutable_record_struct *serialize_mutable_test_type ( mutable_test_type * __value )
				{
					CHECK_NOT_NULL ( __value );
					mutable_record_struct *__data = __value->serialize();
					CHECK_NOT_NULL ( __data );
					return __data;
				}

				/**
				 * Serialize a map instance of key/value type std::string/mutable_test_type * from the passed input data.
				 *
				 * @param __i_map - Input map data.
				 * @return - Serialized data map of key/value type unordered_map<std::string, mutable_record_struct *>
				 */
				std::unordered_map<std::string, mutable_record_struct *> *serialize_map_std__unordered_map_std__string__mutable_test_type___ ( std::unordered_map<std::string, mutable_test_type *> *__i_map )
				{
					CHECK_NOT_EMPTY_P ( __i_map );
					std::unordered_map<std::string, mutable_record_struct *> *__map = new std::unordered_map<std::string, mutable_record_struct *>();
					CHECK_ALLOC ( __map, TYPE_NAME ( unordered_map ) );

					std::unordered_map<std::string, mutable_test_type *>::iterator iter;
					for ( iter = __i_map->begin(); iter != __i_map->end(); iter++ ) {
						const std::string key = iter->first;
						mutable_test_type * value = iter->second;
						CHECK_NOT_NULL ( value );
						mutable_record_struct *rv = this->serialize_mutable_test_type ( value );
						CHECK_NOT_NULL ( rv );
						__map->insert ( {key, rv} );
					}
					return __map;
				}

				/**
				 * Method extracts and set the value of testRefMap from
				 * the serialized data map.
				 *
				 * @param __data - Serialized data map pointer.
				 */
				void set_serde_testRefMap ( mutable_record_struct *__data )
				{
					CHECK_NOT_NULL ( __data );
					if ( NOT_NULL ( this->testRefMap ) ) {
						// Serialize the map testRefMap and add to the data map.
						{
							void *__ptr = serialize_map_std__unordered_map_std__string__mutable_test_type___ ( this->testRefMap );
							CHECK_NOT_NULL ( __ptr );

							const __native_type *ft = get_field_type ( "testRefMap" );
							CHECK_NOT_NULL ( ft );

							__data->add_field ( ft->get_index(), __ptr );
						}

					}
				}


			public:
				/**
				* Set the pointer to name.
				*
				* @param name - Pointer of type char *.
				*/
				void set_name ( char * name )
				{
					this->name = name;
				}

				/**
				 * Get the pointer to the property name of type char *.
				 * Returns a const pointer.
				 *
				 * @return char *
				 */
				const char * get_name ( void ) const
				{
					return this->name;
				}

				/**
				 * Set the char buffer value from the specified string.
				 *
				 * Method should only be used when this instance is being
				 * used to create or update a new record instance.
				 *
				 * @param name - String value to set.
				 */
				void set_name ( const string &name )
				{
					FREE_PTR ( this->name );
					if ( !IS_EMPTY ( name ) ) {
						uint32_t __size = name.length() + 1;
						this->name = ( char * ) malloc ( sizeof ( char ) * __size );
						CHECK_ALLOC ( this->name, TYPE_NAME ( char * ) );
						memset ( this->name, 0, __size );
						memcpy ( this->name, name.c_str(), ( __size - 1 ) );
					}
				}

				/**
				 * Get the pointer to the property testRefMap of type std::unordered_map<std::string, mutable_test_type *> *.
				 * Returns a const pointer.
				 *
				 * @return std::unordered_map<std::string, mutable_test_type *> *
				 */
				const std::unordered_map<std::string, mutable_test_type *> * get_testRefMap ( void ) const
				{
					return this->testRefMap;
				}

				/**
				 * Set the pointer to testRefMap.
				 *
				 * @param testRefMap - Pointer of type std::unordered_map<std::string, mutable_test_type *> *.
				 */
				void set_testRefMap ( std::unordered_map<std::string, mutable_test_type *> * testRefMap )
				{
					this->testRefMap = testRefMap;
				}

				/**
				 * Add a key/value pair to the map with a type value.
				 * The value is expected to be a pre-allocated ${type} pointer.
				 *
				 * Method should only be used when this instance is being
				 * used to create or update a new record instance.
				 *
				 * @param m_key - std::string key value.
				 * @param m_value - mutable_test_type * value pointer.
				 */
				void add_to_testRefMap ( std::string m_key, mutable_test_type * m_value )
				{
					if ( IS_NULL ( this->testRefMap ) ) {
						this->testRefMap = new std::unordered_map<std::string, mutable_test_type *>();
						CHECK_ALLOC ( this->testRefMap, TYPE_NAME ( unordered_map ) );
					}
					this->testRefMap->insert ( {m_key, m_value} );
				}

				/**
				 * Empty constructor when creating an instance of mutable_test_ref_type for setting data locally.
				 * Should be used to create a new record instance of type mutable_test_ref_type.
				 *
				 * @param record_type - Parsed schema definition of this type.
				 */
				mutable_test_ref_type ( const __complex_type *record_type )
				{
					CHECK_NOT_NULL ( record_type );
					this->record_type = record_type;

					this->name = nullptr;
					this->testRefMap = nullptr;

				}

				/**
				 * Copy constructor to create a copy instance of mutable_test_ref_type.
				 * Copy instances should be used to update existing records.
				 *
				 * @param source - Source instance of mutable_test_ref_type to copy from.
				 */
				mutable_test_ref_type ( const test_ref_type &source )
				{
					this->record_type = source.get_record_type();
					CHECK_NOT_NULL ( this->record_type );

					this->name = nullptr;
					this->testRefMap = nullptr;

					// Set name from the source value.
					if ( NOT_NULL ( source.get_name() ) ) {
						string __name = string ( source.get_name() );
						this->set_name ( __name );
					}

// Deserialize the map testRefMap from the source value.
					if ( NOT_EMPTY_P ( source.get_testRefMap() ) ) {
						std::unordered_map<std::string, mutable_test_type *> *__map = new std::unordered_map<std::string, mutable_test_type *>();
						CHECK_ALLOC ( __map, TYPE_NAME ( unordered_map ) );

						const std::unordered_map<std::string, test_type * > *__s_map =  source.get_testRefMap();
						std::unordered_map<std::string, test_type * >::const_iterator iter;
						for ( iter = __s_map->begin(); iter != __s_map->end(); iter++ ) {
							CHECK_NOT_NULL ( iter->second );
							mutable_test_type * __tv = new mutable_test_type ( *iter->second );
							CHECK_ALLOC ( __tv, TYPE_NAME ( mutable_test_type ) );
							__map->insert ( {iter->first, __tv} );
						}
						this->testRefMap = __map;
					}


				}

				/**
				 * Copy constructor to create a copy instance of mutable_test_ref_type.
				 * Copy instances should be used to update existing records.
				 *
				 * @param source - Source instance of mutable_test_ref_type to copy from.
				 */
				mutable_test_ref_type ( const test_ref_type *source )
				{
					this->record_type = source->get_record_type();
					CHECK_NOT_NULL ( this->record_type );

					this->name = nullptr;
					this->testRefMap = nullptr;

					// Set name from the source value.
					if ( NOT_NULL ( source->get_name() ) ) {
						string __name = string ( source->get_name() );
						this->set_name ( __name );
					}

// Deserialize the map testRefMap from the source value.
					if ( NOT_EMPTY_P ( source->get_testRefMap() ) ) {
						std::unordered_map<std::string, mutable_test_type *> *__map = new std::unordered_map<std::string, mutable_test_type *>();
						CHECK_ALLOC ( __map, TYPE_NAME ( unordered_map ) );

						const std::unordered_map<std::string, test_type * > *__s_map =  source->get_testRefMap();
						std::unordered_map<std::string, test_type * >::const_iterator iter;
						for ( iter = __s_map->begin(); iter != __s_map->end(); iter++ ) {
							CHECK_NOT_NULL ( iter->second );
							mutable_test_type * __tv = new mutable_test_type ( iter->second );
							CHECK_ALLOC ( __tv, TYPE_NAME ( mutable_test_type ) );
							__map->insert ( {iter->first, __tv} );
						}
						this->testRefMap = __map;
					}


				}

				/**
				 * Descructor for mutable_test_ref_type
				 */
				~mutable_test_ref_type()
				{
					FREE_PTR ( this->name );

					FREE_TYPE_MAP ( this->testRefMap );


				}

				/**
				 * Serialize this type instance into a data map instance.
				 *
				 * @return - Serialized data record instance.
				 */
				mutable_record_struct *serialize() override
				{
					mutable_record_struct *__data = new mutable_record_struct ( this->record_type );
					CHECK_ALLOC ( __data, TYPE_NAME ( mutable_record_struct ) );
					// Call the method to add the value of name to the serialized data map.
					this->set_serde_name ( __data );

// Call the method to add the value of testRefMap to the serialized data map.
					this->set_serde_testRefMap ( __data );


					return __data;
				}

				/**
				 * Compare this type instance to the passed instance.
				 *
				 * @param __target - Target instance to compare with. If pointer is not
				 * 			of the same type will throw exception.
				 * @return - Is equal?
				 */
				bool equals ( const void* __target ) const override
				{
					if ( IS_NULL ( __target ) ) {
						return false;
					}
					const mutable_test_ref_type * __t = static_cast<const mutable_test_ref_type *> ( __target );
					CHECK_CAST ( __t, TYPE_NAME ( void * ), TYPE_NAME ( mutable_test_ref_type * ) );

					{
						// Compare field name
						const __native_type *nt = get_field_type ( "name" );
						CHECK_NOT_NULL ( nt );
						__base_datatype_io *th = nt->get_type_handler ( __record_mode::RM_READ );
						CHECK_NOT_NULL ( th );
						const void *tv = __t->get_name();
						const void *sv = this->get_name();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
							return true;
						}
						bool r = th->compare ( tv, sv, __constraint_operator::EQ );
						if ( !r ) {
							return false;
						}
					}

					{
						// Compare map instance testRefMap
						const unordered_map<std::string, mutable_test_type *> *tv = __t->get_testRefMap();
						const unordered_map<std::string, mutable_test_type *> *sv = this->get_testRefMap();
						if ( IS_NULL ( sv ) ) {
							if ( NOT_NULL ( tv ) ) {
								return false;
							}
							return true;
						}
						if ( tv->size() != sv->size() ) {
							return false;
						}

						unordered_map<std::string, mutable_test_type *>::const_iterator iter_t;
						unordered_map<std::string, mutable_test_type *>::const_iterator iter_s;

						for ( iter_s = sv->begin(); iter_s != sv->end(); iter_s++ ) {
							iter_t = tv->find ( iter_s->first );
							if ( iter_t == tv->end() ) {
								return false;
							}
							const mutable_test_type * v1 = iter_s->second;
							const mutable_test_type * v2 = iter_t->second;
							if ( IS_NULL ( v1 ) ) {
								if ( NOT_NULL ( v2 ) ) {
									return false;
								}
								continue;
							}
							// Check value type equals.
							bool r = v1->equals ( v2 );
							if ( !r ) {
								return false;
							}
						}
					}



					return true;
				}


			};

		}

	}

}

#endif // TEST_REF_TYPE_H

