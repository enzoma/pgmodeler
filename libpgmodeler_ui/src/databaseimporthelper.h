/*
# PostgreSQL Database Modeler (pgModeler)
#
# Copyright 2006-2013 - Raphael Araújo e Silva <rkhaotix@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# The complete text of GPLv3 is at LICENSE file on source code root directory.
# Also, you can get the complete GNU General Public License at <http://www.gnu.org/licenses/>
*/

/**
\ingroup libpgmodeler_ui
\class DatabaseImportHelper
\brief Implements the base operations to import existing databases into model (reverse engineering)
*/

#ifndef DATABASE_IMPORT_HELPER_H
#define DATABASE_IMPORT_HELPER_H

#include <QObject>
#include <QThread>
#include "catalog.h"
#include "modelwidget.h"

class DatabaseImportHelper: public QObject {
	private:
		Q_OBJECT

		//This pattern matches the array of oids in format [n:n]={a,b,c,d,...} or {a,b,c,d,...}
		static const QString ARRAY_PATTERN;

		Catalog catalog;

		Connection connection;

		bool import_canceled, ignore_errors, import_sys_objs;

		map<ObjectType, vector<unsigned>> object_oids;
		map<unsigned, vector<unsigned>> column_oids;

		vector<unsigned> creation_order;

		map<unsigned, BaseObject *> created_objs;
		map<unsigned, attribs_map> user_objs;
		map<unsigned, attribs_map> system_objs;

		ModelWidget *model_wgt;

		DatabaseModel *dbmodel;

		void configureDatabase(attribs_map &attribs);
		void createObject(attribs_map &attribs);
		void createSchema(attribs_map &attribs);
		void createRole(attribs_map &attribs);
		void createDomain(attribs_map &attribs);
		void createExtension(attribs_map &attribs);
		void createFunction(attribs_map &attribs);
		void createLanguage(attribs_map &attribs);
		void createOperatorFamily(attribs_map &attribs);
		void createOperatorClass(attribs_map &attribs);
		void createOperator(attribs_map &attribs);

		QStringList parseArrayValues(const QString array_val);
		QStringList parseDefaultValues(const QString &def_vals);

		QString getObjectName(unsigned oid, bool signature_form=false);
		QStringList getObjectNames(const QString &oid_vect, bool signature_form=false);

		QString getType(unsigned oid, bool generate_xml, attribs_map extra_attribs=attribs_map());
		QStringList getTypes(const QString &oid_vect, bool generate_xml);

		QString getDependencyObject(unsigned oid, bool use_signature=false, attribs_map extra_attribs=attribs_map());
		QString getComment(attribs_map &attribs);

		void loadObjectXML(ObjectType obj_type, attribs_map &attribs);

	public:
		DatabaseImportHelper(QObject *parent=0);
		~DatabaseImportHelper(void);

		void setConnection(Connection &conn);
		void setCurrentDatabase(const QString &dbname);
		void setImportSystemObject(bool value);
		void setIgnoreErrors(bool value);
		void setSelectedOIDs(ModelWidget *model_wgt, map<ObjectType, vector<unsigned>> &obj_oids, map<unsigned, vector<unsigned>> &col_oids);
		unsigned getLastSystemOID(void);

		/*! \brief Returns an attribute map for the specified object type. The parameters "schema" and "table"
				must be used only when retrieving table children objects.
				\note: The database used as reference is the same as the currently connection. So,
				if the user want a different database it must call Connection::switchToDatabase() method
				before assigne the connection to this class. */
		attribs_map getObjects(ObjectType obj_type, const QString &schema="", const QString &table="", attribs_map extra_attribs=attribs_map());


    signals:
		//! \brief This singal is emitted whenever the export progress changes
		void s_progressUpdated(int progress, QString msg, ObjectType obj_type=BASE_OBJECT);

		//! \brief This signal is emited when the import has finished
		void s_importFinished(Exception e=Exception());

		//! \brief This signal is emited when the import has been cancelled
		void s_importCanceled(void);

		//! \brief This signal is emited when the import has encountered a critical error (only in thread mode)
		void s_importAborted(Exception e);

	protected slots:
		void cancelImport(void);

	public slots:
		void importDatabase(void);
		
	friend class DatabaseImportForm;
};

#endif