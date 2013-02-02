/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 


#ifndef CTHREADPOOLJOB_HPP_
#define CTHREADPOOLJOB_HPP_

/**
 * Base interface for additional job data container.
 */
class IThreadPoolJobData
{
public:
   virtual ~IThreadPoolJobData() {}
};

/**
 * Class for job representation.
 * Contains the function pointer to the callback which will perform the job.
 * It is possible to pass additional data as struct inherited from
 * class IThreadPoolJobData.
 */
class CThreadPoolJob
{
public:
   // types section

   typedef void (*tJobPerformer)(IThreadPoolJobData const* job);

public:
   // method section

   /**
    * Constructor for job.
    * @param pFunc is the pointer to function which will perform this job
    * @param pData is the pointer to additional data. Data MUST be allocated
    * with default new. It will be destroyed right after job perform or job 
    * destruction at thread pool destruction. After you passed job object to 
    * pool, you MUST forget about this data - it belongs to you no more.
    */
   CThreadPoolJob(tJobPerformer pFunc, IThreadPoolJobData* pData = 0);

   /**
    * Copy constructor
    * @param job reference
    */
   CThreadPoolJob(CThreadPoolJob const& job);

   /**
    * Destructor
    */
   ~CThreadPoolJob();

   /**
    * Uses passed function pointer to perform the job.
    */
   void perform();

   /**
    * Destroys the passed job data.
    */
   void destroyData();

private:
   // members section

   tJobPerformer mPerformer;      /// Pointer to job performer function
   IThreadPoolJobData* mpJobData; /// Pointer to additional data for job
};


#endif /* CTHREADPOOLJOB_HPP_ */
